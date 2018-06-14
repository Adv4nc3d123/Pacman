#include "MiniginPCH.h"
#include "InputManager.h"
#include <SDL.h>


bool dae::InputManager::ProcessInput()
{
	ZeroMemory(&currentState, sizeof(XINPUT_STATE));
	XInputGetState(0, &currentState);

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			return false;
		}
		if (e.type == SDL_KEYDOWN) {
			if (IsDown(Key::KeyA, e))
				Key_A->Execute();
			if (IsDown(Key::KeyD, e))
				Key_D->Execute();
			if (IsDown(Key::KeyS, e))
				Key_S->Execute();
			if (IsDown(Key::KeyW, e))
				Key_W->Execute();

		}
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			
		}
	}
	if (IsPressed(ControllerButton::ButtonA))
		Button_A->Execute();
	if (IsPressed(ControllerButton::ButtonX))
		Button_X->Execute();
	if (IsPressed(ControllerButton::ButtonY))
		Button_Y->Execute();
	if (IsPressed(ControllerButton::ButtonB))
		Button_B->Execute();
	if (IsPressed(ControllerButton::DpadDown))
		Dpad_Down->Execute();
	if (IsPressed(ControllerButton::DpadLeft))
		Dpad_Left->Execute();
	if (IsPressed(ControllerButton::DpadRight))
		Dpad_Right->Execute();
	if (IsPressed(ControllerButton::DpadUp))
		Dpad_Up->Execute();


	return true;
}

bool dae::InputManager::IsPressed(ControllerButton button) const
{
	switch (button)
	{
	case ControllerButton::ButtonA:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
	case ControllerButton::ButtonB:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B;
	case ControllerButton::ButtonX:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_X;
	case ControllerButton::ButtonY:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
	case ControllerButton::DpadDown:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
	case ControllerButton::DpadLeft:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
	case ControllerButton::DpadRight:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
	case ControllerButton::DpadUp:
		return currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
	default: 
		return false;
	}
}

bool dae::InputManager::IsDown(Key key, const SDL_Event& e) const
{
	switch (key)
	{
	case Key::KeyA:
		return (e.key.keysym.sym == SDLK_a);
	case Key::KeyD:
		return (e.key.keysym.sym == SDLK_d);
	case Key::KeyS:
		return (e.key.keysym.sym == SDLK_s);
	case Key::KeyW:
		return (e.key.keysym.sym == SDLK_w);
	default:
		throw std::runtime_error(std::string("impossible input") + SDL_GetError());
	}
}


void dae::InputManager::SetCommand(ControllerButton button, Command* command)
{
	switch(button)
	{
	case ControllerButton::ButtonA:
		Button_A = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::ButtonB:
		Button_B = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::ButtonX:
		Button_X = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::ButtonY:
		Button_Y = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::DpadDown:
		Dpad_Down = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::DpadLeft:
		Dpad_Left = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::DpadRight:
		Dpad_Right = std::unique_ptr<Command>(command);
		break;
	case ControllerButton::DpadUp:
		Dpad_Up = std::unique_ptr<Command>(command);
		break;
	default:
		throw std::runtime_error(std::string("impossible input") + SDL_GetError());
	}
}

void dae::InputManager::SetCommand(Key key, Command* command)
{
	switch (key)
	{
	case Key::KeyW:
		Key_W = std::unique_ptr<Command>(command);
		break;
	case Key::KeyS:
		Key_S = std::unique_ptr<Command>(command);
		break;
	case Key::KeyA:
		Key_A = std::unique_ptr<Command>(command);
		break;
	case Key::KeyD:
		Key_D = std::unique_ptr<Command>(command);
		break;
	default:
		throw std::runtime_error(std::string("impossible input") + SDL_GetError());

	}
}

