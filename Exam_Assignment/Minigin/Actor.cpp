#include "MiniginPCH.h"
#include "Actor.h"
#include <SDL.h>
#include "ImageComponent.h"
#include "ResourceManager.h"
#include "Level.h"
#include "Tile.h"
#include "Score.h"
#include "HitBoxComponent.h"
#include "Ghost.h"
#include "Health.h"

Actor::Actor(const std::shared_ptr<Score>& score, const std::shared_ptr<Level>& level, const std::shared_ptr<Health>& health)
	: GameObject()
	, m_Speed(100.0f)
	, m_HorSpeed(0.0f)
	, m_VerSpeed(0.0f)
	, m_Direction(Direction::Right)
	, m_NewDirection(Direction::Right)
	, m_Tile(0)
	, m_Turn(false)
	, m_TurnPos(0, 0, 0)
	, m_FrameTime(0.0f)
	, m_FrameNr(2)
	, m_Frames(3)
	, m_Score(score)
	, m_Level(level)
	, m_StartPos(9 * 32, 16 * 32, 0)
	, m_Health(health)
{
}

void Actor::Update(const float deltaTime)
{
	//auto hit = OnOverlap(std::make_shared<Ghost>(
	//	std::make_shared<Actor>(std::make_shared<Score>(), std::make_shared<Level>()), std::make_shared<Level>())); 
	//if(hit)
	//{
	//	Hit();
	//}
	UpdateTexture(deltaTime);
	EatDots();
	if(m_NewDirection != m_Direction)
		CheckDirection(m_NewDirection);

	auto pos = m_Transform.GetPosition();
	auto xPos = float(pos.x + m_HorSpeed * deltaTime);
	auto yPos = float(pos.y + m_VerSpeed * deltaTime);
	SetPosition(xPos, yPos);
	HandleCollision(); //call after setposition to snap back if setposition goes too far
	float threshHold{ 3.0f };
	if(m_Turn)
	{
		if((m_TurnPos.x > pos.x - threshHold/2.0f) && (m_TurnPos.y > pos.y - threshHold/2.0f) &&
			(m_TurnPos.x < pos.x + threshHold / 2.0f) && (m_TurnPos.y < pos.y + threshHold/2.0f))
		{
			m_Transform.SetPosition(m_TurnPos.x, m_TurnPos.y);
			ChangeDirection(m_NewDirection);
			m_Turn = false;
		}
	}



}

void Actor::Initialize()
{
	m_Textures[0] = "Pacman_Open.png";
	m_Textures[1] = "Pacman_HalfOpen.png";
	m_Textures[2] = "Pacman_Closed.png";

	auto texture = std::make_shared<ImageComponent>();
	texture->SetTexture(m_Textures[1]);
	SetPosition(m_StartPos.x, m_StartPos.y);
	AddComponent(texture);

	auto hitbox = std::make_shared<HitBoxComponent>();
	hitbox->SetHeight(30.0f);
	hitbox->SetWidth(30.0f);
	AddComponent(hitbox);

}

void Actor::SetLevel(const std::shared_ptr<Level>& level)
{
	m_Level = level;
}


void Actor::SetDirection(Direction direction)
{
	m_NewDirection = direction;
}

int Actor::GetTileIndex()
{
	return m_Level->GetActiveTileIndex(shared_from_this());
}

void Actor::Hit()
{
	m_Health->DecreaseHealth(1);
	SetPosition(m_StartPos.x, m_StartPos.y);
	m_HorSpeed = 0.0f;
	m_VerSpeed = 0.0f;
	m_Direction = Direction::Right;
}

void Actor::AddGhost(const std::weak_ptr<Ghost>& ghost)
{
	m_Ghosts.push_back(ghost);
}

void Actor::HandleCollision()
{
	auto indexPos = m_Level->GetActiveTileIndex(shared_from_this());
	auto OnTile = m_Level->GetTile(indexPos);
	int nearIndex;
	std::shared_ptr<Tile> nearTile;

	switch (m_Direction)
	{
	case Direction::Up:
		nearIndex = m_Level->GetUpperIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}
		break;
	case Direction::Down:
		nearIndex = m_Level->GetBottomIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}

		break;
	case Direction::Left:
		nearIndex = m_Level->GetLeftIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}

		break;
	case Direction::Right:
		nearIndex = m_Level->GetRightIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}

		break;
	}
}

void Actor::UpdateTexture(const float deltaTime)
{
	m_FrameTime += deltaTime;
	if(m_FrameTime > (0.1f))
	{
		m_FrameTime = 0.0f;
		m_FrameNr = (m_FrameNr + 1) % m_Frames;
		auto ic = GetComponent<ImageComponent>();
		ic->SetTexture(m_Textures[m_FrameNr]);
	}
}

bool Actor::CheckDirection(Direction direction)
{
	if(m_Direction != direction)
	{
		auto indexPos = m_Level->GetActiveTileIndex(shared_from_this());
		auto OnTile = m_Level->GetTile(indexPos);
		int nearIndex;
		std::shared_ptr<Tile> nearTile;
		switch(direction)
		{
			case Direction::Up:
				nearIndex = m_Level->GetUpperIndex(indexPos);
				nearTile = m_Level->GetTile(nearIndex);
				if(nearTile->GetState() != TileState::Wall)
				{
					m_Turn = true;
					m_TurnPos = OnTile->GetTransform().GetPosition();
					m_NewDirection = direction;
				}
				break;
			case Direction::Left:
				nearIndex = m_Level->GetLeftIndex(indexPos);
				nearTile = m_Level->GetTile(nearIndex);
				if (nearTile->GetState() != TileState::Wall)
				{
					m_Turn = true;
					m_TurnPos = OnTile->GetTransform().GetPosition();
					m_NewDirection = direction;
				}
				break;
			case Direction::Right:
				nearIndex = m_Level->GetRightIndex(indexPos);
				nearTile = m_Level->GetTile(nearIndex);
				if (nearTile->GetState() != TileState::Wall)
				{
					m_Turn = true;
					m_TurnPos = OnTile->GetTransform().GetPosition();
					m_NewDirection = direction;
				}
				break;
			case Direction::Down:
				nearIndex = m_Level->GetBottomIndex(indexPos);
				nearTile = m_Level->GetTile(nearIndex);
				if (nearTile->GetState() != TileState::Wall)
				{
					m_Turn = true;
					m_TurnPos = OnTile->GetTransform().GetPosition();
					m_NewDirection = direction;
				}
				break;
		
		}

		//no snapping if just you just turn around in your lane
		if((m_Direction == Direction::Down && direction == Direction::Up) ||
			(m_Direction == Direction::Up && direction == Direction::Down) ||
			(m_Direction == Direction::Left && direction == Direction::Right) ||
			(m_Direction == Direction::Right && direction == Direction::Left))
		{
			m_TurnPos = m_Transform.GetPosition();
		}



		return true;
	}
	return false;
}

void Actor::ChangeDirection(Direction direction)
{
	m_Direction = direction;
	switch (m_Direction)
	{
	case Direction::Up:
		m_Transform.SetAngle(-90.0f);
		m_HorSpeed = 0.0f;
		m_VerSpeed = -m_Speed;
		break;
	case Direction::Down:
		m_Transform.SetAngle(90.0f);
		m_HorSpeed = 0.0f;
		m_VerSpeed = m_Speed;
		break;
	case Direction::Left:
		m_Transform.SetAngle(180.0f);
		m_HorSpeed = -m_Speed;
		m_VerSpeed = 0.0f;
		break;
	case Direction::Right:
		m_Transform.SetAngle(0.0f);
		m_HorSpeed = m_Speed;
		m_VerSpeed = 0.0f;
		break;
	default:
		throw std::runtime_error(std::string("impossible direction") + SDL_GetError());
	}
}

void Actor::EatDots()
{
	auto indexPos = m_Level->GetActiveTileIndex(shared_from_this());
	auto OnTile = m_Level->GetTile(indexPos);
	if(OnTile->GetState() == TileState::Dot)
	{
		OnTile->SetState(TileState::Open);
		m_Score->IncreaseScore(5);
	}
	if (OnTile->GetState() == TileState::Boost)
	{
		OnTile->SetState(TileState::Open);
		for(auto ghost : m_Ghosts)
		{
			ghost.lock()->SetState(ActorState::Scared);
		}
	}

}
