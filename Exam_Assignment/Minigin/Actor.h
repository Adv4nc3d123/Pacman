#pragma once
#include "GameObject.h"

class Texture2D;
class Level;
class Score;
class Health;
class Ghost;

enum class Direction
{
	Up, 
	Down,
	Left,
	Right
};


class Actor : public GameObject
{
public:
	Actor(const std::shared_ptr<Score>& score, const std::shared_ptr<Level>& level, const std::shared_ptr<Health>& health);
	void Update(const float deltaTime) override;
	void Initialize();
	void SetLevel(const std::shared_ptr<Level>& level);
	void SetDirection(Direction direction);
	int GetTileIndex();
	void Hit();
	void AddGhost(const std::weak_ptr<Ghost>& ghost);
private:
	std::shared_ptr<Health> m_Health;
	int m_Tile;
	float m_HorSpeed;
	float m_VerSpeed;
	const float m_Speed;
	Direction m_Direction;
	Direction m_NewDirection;
	void HandleCollision();
	void UpdateTexture(const float deltaTime);
	bool CheckDirection(Direction direction);
	void ChangeDirection(Direction direction);
	void EatDots();
	std::string m_Textures[3];
	std::shared_ptr<Level> m_Level;
	bool m_Turn;
	glm::vec3 m_TurnPos;
	float m_FrameTime;
	int m_FrameNr;
	int m_Frames;
	std::shared_ptr<Score> m_Score;
	glm::vec3 m_StartPos;
	
	std::vector<std::weak_ptr<Ghost>> m_Ghosts;
};
