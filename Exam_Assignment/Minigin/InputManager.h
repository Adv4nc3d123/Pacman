#pragma once
#include <XInput.h>
#include "Singleton.h"
#include "Command.h"
#include <SDL.h>

namespace dae
{
	enum class ControllerButton
	{
		ButtonA,
		ButtonB,
		ButtonX,
		ButtonY, 
		DpadDown,
		DpadRight,
		DpadUp,
		DpadLeft
	};
	enum class Key
	{
		KeyW,
		KeyS,
		KeyA,
		KeyD
	};

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();
		bool IsPressed(ControllerButton button) const;
		bool IsDown(Key key, const SDL_Event&e) const;
		void SetCommand(ControllerButton button, Command* command);
		void SetCommand(Key key, Command* command);
	private:
		XINPUT_STATE currentState{};
		
		std::unique_ptr<Command> Button_A;
		std::unique_ptr<Command> Button_B;
		std::unique_ptr<Command> Button_X;
		std::unique_ptr<Command> Button_Y;
		std::unique_ptr<Command> Dpad_Down;
		std::unique_ptr<Command> Dpad_Right;
		std::unique_ptr<Command> Dpad_Up;
		std::unique_ptr<Command> Dpad_Left;
		std::unique_ptr<Command> Key_W;
		std::unique_ptr<Command> Key_S;
		std::unique_ptr<Command> Key_A;
		std::unique_ptr<Command> Key_D;

	};

}
