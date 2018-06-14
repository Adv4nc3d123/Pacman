#pragma once
#include "GameObject.h"
#include <set>
#include "Actor.h"

class Level;
class Actor;

enum class ActorState
{
	Normal,
	Scared,
	Dead
};


enum class GDirection
{
	Up,
	Down,
	Left,
	Right
};

class Ghost : public GameObject
{
public:
	Ghost(const std::shared_ptr<Actor>& actor, const std::shared_ptr<Level>& level);
	void Hit();
	void Update(const float deltaTime) override;
	int CreatePath(int src, int target);
	void StorePath();
	int GetTileIndex();
	void Initialize();
	void SetState(ActorState state);

private:
	void Move(int dest);
	Direction m_Direction;
	void HandleCollision();
	void ChangeDirection(Direction direction);
	std::vector<std::pair<int, int>> m_Path;
	std::vector<int> m_ActualPath;
	int m_PathIndex;
	float m_Speed;
	float m_HorSpeed;
	float m_VerSpeed;
	int m_Tile;
	int m_NextTile;
	ActorState m_ActorState;
	std::shared_ptr<Level> m_Level;
	std::shared_ptr<Actor> m_Actor;
	std::string m_Textures[3];
};
