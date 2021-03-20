// The Tofita Kernel
// Copyright (C) 2020  Oleg Petrenko
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

namespace process {

function Process_init(Process *process) {
	process->pml4 = pages::newCR3(processes[0].pml4);
	process->schedulable = false;						// Not yet ready
	process->scheduleOnNextTick = false;				// Prevent DoS attack
	memset(&process->frame, 0, sizeof(InterruptFrame)); // Zeroing
	process->is64bit = true;
	process->limits = pages::AddressAwareness::Bit64limit8TB;
	process->frame.cs = USER_CODE64_SEL + 3;
	process->frame.ss = USER_DATA32_SEL + 3;
	// process->frame.flags = 0x002; // No interrupts
	process->frame.flags = 0x202;
	process->syscallToHandle = TofitaSyscalls::Noop;
	process->frame.rdxArg1 = process->pid; // Second argument
	process->messages = null;
	process->awaitsGetMessage = false;
}

Process *Process_create() {
	uint64_t index = 0;
	while (index < 255) {
		index++;
		if (index == 256)
			return null;
		if (processes[index].present != true)
			break;
	}

	Process *process = &process::processes[index];
	process->pid = index;
	process->present = true; // Occupied

	return process;
}

// Note: supposedly 10.000 system-wise
constexpr uint64_t messagesBufferSize = 256;

wapi::Msg *getOrCreateMessagesBuffer(volatile Process *process) {
	if (process->messages == null) {
		constexpr uint64_t bytes = sizeof(wapi::Msg) * messagesBufferSize;

		process->messages = (wapi::Msg *)PhysicalAllocator::allocateBytes(bytes);

		if (process->messages != PHYSICAL_NOT_FOUND) {
			memset((void *)process->messages, 0, bytes);
			process->messagesUsed = 0;
		} else {
			process->messages = null;
			return null;
		}
	}

	return process->messages;
}

bool postMessage(volatile Process *process, PostMessagePayload *payload) {
	// TODO do not allocate messages queue if no windows and not awaits for them
	var messages = getOrCreateMessagesBuffer(process);
	if (messages == null)
		return false;

	if (process->messagesUsed == messagesBufferSize)
		return false;

	wapi::Msg *message = &messages[process->messagesUsed];

	message->hwnd = payload->hWnd;
	message->message = payload->msg;
	message->wParam = payload->wParam;
	message->lParam = payload->lParam;

	message->time = uptimeMilliseconds; // TODO
	message->pt.x = dwm::mouseX;		// TODO
	message->pt.y = dwm::mouseY;		// TODO
	message->lPrivate = 0;

	process->messagesUsed++;

	if (process->awaitsGetMessage) {
		process->awaitsGetMessage = false;
		process->syscallToHandle = TofitaSyscalls::GetMessage;
	}

	return true;
}

bool getMessage(volatile Process *process, GetMessagePayload *payload) {
	// Do not allocate until needed
	if (process->messages == null)
		return false;

	var messages = getOrCreateMessagesBuffer(process);
	if (messages == null)
		return false;

	// TODO does not apply to wmQuit/wmPaint
	if (process->messagesUsed == 0)
		return false;

	// TODO filters & hWnd
	wapi::Msg *message = &messages[0];
	wapi::Msg *target = payload->msg;

	target->hwnd = message->hwnd;
	target->message = message->message;
	target->wParam = message->wParam;
	target->lParam = message->lParam;

	target->time = message->time;
	target->pt.x = message->pt.x;
	target->pt.y = message->pt.y;
	target->lPrivate = message->lPrivate;

	// Shift first element
	process->messagesUsed--;

	for (uint16_t i = 0; i < process->messagesUsed; ++i) {
		messages[i] = messages[i + 1];
	}

	return true;
}

function Process_destroy(volatile Process *process) {
	// TODO deallocate stuff
	process->present = false;
	process->messages = null;

	var window = dwm::OverlappedWindow_findAnyProcessWindow(process->pid);

	while (window != null) {
		dwm::OverlappedWindow_destroy(window->windowId);
		window = dwm::OverlappedWindow_findAnyProcessWindow(process->pid);
	}
}

} // namespace process
