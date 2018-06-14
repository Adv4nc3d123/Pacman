#include "MiniginPCH.h"
#include "Minigin.h"
#include <thread>
#include "InputManager.h"
#include "SceneManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include <SDL.h>
#include "GameObject.h"
#include "Scene.h"
#include "FPS_Counter.h"
#include "TurnLeftCommand.h"
#include "Actor.h"
#include "TurnDownCommand.h"
#include "TurnRightCommand.h"
#include "TurnUpCommand.h"
#include "Level.h"
#include "Score.h"
#include "Ghost.h"
#include "Health.h"


void dae::Minigin::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) 
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	window = SDL_CreateWindow(
		"Programming 4 assignment",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		608,
		704,
		SDL_WINDOW_OPENGL
	);
	if (window == nullptr) 
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}
	Renderer::GetInstance().Init(window);
}

/**
 * Code constructing the scene world starts here
 */
void dae::Minigin::LoadGame() const
{
	auto& scene = SceneManager::GetInstance().CreateScene("test");

	//Level
	auto level = std::make_shared<Level>();

	//get window size
	int width = 0;
	int height = 0;
	SDL_GetWindowSize(window, &width, &height);

	//FPS_Counter
	auto FPS_Counter = std::make_shared<::FPS_Counter>();
	FPS_Counter->Initialize();
	FPS_Counter->SetPosition(width-50.0f, 0.0f);

	//Score
	auto score = std::make_shared<Score>(); //create score here and not as child of pacman (child will move around relative to parent)
	score->Initialize();
	score->SetPosition(0, 0);

	//Health
	auto health = std::make_shared<Health>();
	health->Initialize();
	health->SetPosition(width/2.0f - 10.0f, 0);

	//Pacman
	auto actor = std::make_shared<Actor>(score, level, health);
	actor->Initialize();
	LoadCommands(actor);

	//Ghost //TODO: Multithreading for ghosts
	auto ghost = std::make_shared<Ghost>(actor, level);
	ghost->Initialize();

	//auto ghost2 = std::make_shared<Ghost>(actor, level);
	//ghost2->Initialize();

	actor->AddGhost(ghost);
	//actor->AddGhost(ghost2);

	scene.Add(level);
	scene.Add(actor);
	scene.Add(ghost);
	//scene.Add(ghost2);
	scene.Add(FPS_Counter);
	scene.Add(score);
	scene.Add(health);

}

void dae::Minigin::Cleanup()
{
	Renderer::GetInstance().Destroy();
	SDL_DestroyWindow(window);
	window = nullptr;
	SDL_Quit();
}

void dae::Minigin::Run()
{
	Initialize();
	m_T1 = std::chrono::high_resolution_clock::now();

	// tell the resource manager where he can find the game data
	ResourceManager::GetInstance().Init("../Data/");

	LoadGame();

	{
		auto& renderer = Renderer::GetInstance();
		auto& sceneManager = SceneManager::GetInstance();
		auto& input = InputManager::GetInstance();
		bool doContinue = true;
		while (doContinue)
		{
			auto t2 = std::chrono::high_resolution_clock::now();
			auto timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - m_T1);
			float deltaTime = (float)timeSpan.count();

			//otherwise actor could jump to impossible locations after a breakpoint or huge timegap
			const float maxDelta{ 0.1f };
			deltaTime = min(deltaTime, maxDelta);
			m_T1 = t2;

			doContinue = input.ProcessInput();


			
			sceneManager.Update(deltaTime);;
			renderer.Render();

		}
	}

	Cleanup();
}

void dae::Minigin::LoadCommands(const std::shared_ptr<Actor>& actor) const
{
	auto& input = InputManager::GetInstance();
	input.SetCommand(ControllerButton::DpadLeft, new TurnLeftCommand(actor));
	input.SetCommand(Key::KeyA, new TurnLeftCommand(actor));
	input.SetCommand(ControllerButton::DpadDown, new TurnDownCommand(actor));
	input.SetCommand(Key::KeyS, new TurnDownCommand(actor));
	input.SetCommand(ControllerButton::DpadRight, new TurnRightCommand(actor));
	input.SetCommand(Key::KeyD, new TurnRightCommand(actor));
	input.SetCommand(ControllerButton::DpadUp, new TurnUpCommand(actor));
	input.SetCommand(Key::KeyW, new TurnUpCommand(actor));

	
}
