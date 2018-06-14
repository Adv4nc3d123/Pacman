#include "MiniginPCH.h"
#include "Ghost.h"
#include "Level.h"
#include "Tile.h"
#include <set>
#include "ImageComponent.h"
#include "Actor.h"
#include <algorithm>
#include "HitBoxComponent.h"
#include <SDL.h>

struct edge //used for dijkstra
{
	int target;
	int weight;
};


Ghost::Ghost(const std::shared_ptr<Actor>& actor, const std::shared_ptr<Level>& level)
	:m_Level(level)
	, m_Actor(actor)
	, m_Speed(80.0f)
	, m_HorSpeed(0.0f)
	, m_VerSpeed(0.0f)
	, m_ActorState(ActorState::Normal)
{
}

void Ghost::Hit()
{
	m_HorSpeed = 0.0f;
	m_VerSpeed = 0.0f;
	m_ActorState = ActorState::Normal;
	Initialize();
}

void Ghost::Initialize()
{
	m_Textures[0] = "Ghost_Normal.png";
	m_Textures[1] = "Ghost_Scared.png";
	m_Textures[2] = "Ghost_Dead.png";


	auto texture = std::make_shared<ImageComponent>();
	texture->SetTexture(m_Textures[0]);
	SetPosition(32,32);
	AddComponent(texture);

	auto hitbox = std::make_shared<HitBoxComponent>();
	hitbox->SetHeight(30.0f);
	hitbox->SetWidth(30.0f);
	AddComponent(hitbox);

	m_Tile = GetTileIndex();
	CreatePath(GetTileIndex(), m_Actor->GetTileIndex());
	StorePath();
	m_NextTile = m_ActualPath[0];

	m_HorSpeed = 0.0f;
	m_VerSpeed = m_Speed;

}


void Ghost::Update(const float deltaTime)
{
	if (m_ActorState != ActorState::Dead)
	{
		int random = rand() % m_Level->GetTiles().size();

		switch (m_ActorState)
		{
		case ActorState::Normal:
			GetComponent<ImageComponent>()->SetTexture(m_Textures[0]);

			Move(m_Actor->GetTileIndex());
			if (GetComponent<HitBoxComponent>()->IsOverLapping(m_Actor->GetComponent<HitBoxComponent>()))
			{
				m_Actor->Hit();
				Hit();
			}
			break;
		case ActorState::Scared:
			GetComponent<ImageComponent>()->SetTexture(m_Textures[1]);
			while (m_Level->GetTile(random)->GetState() == TileState::Wall)
			{
				random = rand() % m_Level->GetTiles().size();
			}
			Move(random);
			if (GetComponent<HitBoxComponent>()->IsOverLapping(m_Actor->GetComponent<HitBoxComponent>()))
			{
				m_ActorState = ActorState::Dead;
				GetComponent<ImageComponent>()->SetTexture(m_Textures[2]);
			}
			break;
		}
		auto Oldpos = m_Transform.GetPosition();
		auto xPos = float(Oldpos.x + m_HorSpeed * deltaTime);
		auto yPos = float(Oldpos.y + m_VerSpeed * deltaTime);
		SetPosition(xPos, yPos);
	}

}

void Ghost::Move(int dest)
{
	auto pos = m_Transform.GetPosition();
	float threshHold{ 3.0f };

	m_Tile = GetTileIndex();

	if (m_Tile == m_NextTile)
	{
		auto turnPos = m_Level->GetTile(m_NextTile)->GetTransform().GetPosition();
		if ((turnPos.x > pos.x - threshHold / 2.0f) && (turnPos.y > pos.y - threshHold / 2.0f) &&
			(turnPos.x < pos.x + threshHold / 2.0f) && (turnPos.y < pos.y + threshHold / 2.0f))
		{
			m_Transform.SetPosition(turnPos.x, turnPos.y);

			CreatePath(GetTileIndex(), dest);
			StorePath();
			m_NextTile = m_ActualPath[0];

			auto tile = m_Level->GetTile(m_NextTile);
			auto pos3 = tile->GetTransform().GetPosition();

			auto pos2 = m_Level->GetTile(m_Tile)->GetTransform().GetPosition();


			auto moveVector = pos3 - pos2;


			if (moveVector.x > 0.0f)
				ChangeDirection(Direction::Right);
			if (moveVector.x < 0.0f)
				ChangeDirection(Direction::Left);
			if (moveVector.y > 0.0f)
				ChangeDirection(Direction::Down);
			if (moveVector.y < 0.0f)
				ChangeDirection(Direction::Up);

		}
	}
}

void Ghost::SetState(ActorState state)
{
	m_ActorState = state;
}

void Ghost::ChangeDirection(Direction direction)
{
	m_Direction = direction;
	switch (m_Direction)
	{
	case Direction::Up:
		m_HorSpeed = 0.0f;
		m_VerSpeed = -m_Speed;
		break;
	case Direction::Down:
		m_HorSpeed = 0.0f;
		m_VerSpeed = m_Speed;
		break;
	case Direction::Left:
		m_HorSpeed = -m_Speed;
		m_VerSpeed = 0.0f;
		break;
	case Direction::Right:
		m_HorSpeed = m_Speed;
		m_VerSpeed = 0.0f;
		break;

	}
}


void Ghost::HandleCollision()
{
	auto indexPos = m_Level->GetActiveTileIndex(shared_from_this());
	auto OnTile = m_Level->GetTile(indexPos);
	int nearIndex;
	std::shared_ptr<Tile> nearTile;

		nearIndex = m_Level->GetUpperIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}
		nearIndex = m_Level->GetBottomIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}
		nearIndex = m_Level->GetLeftIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}

		nearIndex = m_Level->GetRightIndex(indexPos);
		nearTile = m_Level->GetTile(nearIndex);
		if (nearTile->GetState() == TileState::Wall && nearTile->IsOverLapping(GetComponent<HitBoxComponent>()))
		{
			m_Transform.SetPosition(OnTile->GetTransform().GetPosition().x, OnTile->GetTransform().GetPosition().y);
		}
}

int Ghost::CreatePath(int src, int target)
{
	m_Path.clear();

	//Generate all possible edges
	std::vector<std::vector<edge>> levelEdges;
	levelEdges.resize(m_Level->GetTiles().size());

	for (int i{ 0 }; i < levelEdges.size(); ++i)
	{
		int levelIndex = m_Level->GetRightIndex(i);

		if(m_Level->GetTile(levelIndex)->GetState() != TileState::Wall)
			levelEdges[i].push_back(edge{ levelIndex, 1 });

		levelIndex = m_Level->GetBottomIndex(i);

		if (m_Level->GetTile(levelIndex)->GetState() != TileState::Wall)
			levelEdges[i].push_back(edge{ levelIndex, 1 });

		levelIndex = m_Level->GetLeftIndex(i);

		if (m_Level->GetTile(levelIndex)->GetState() != TileState::Wall)
			levelEdges[i].push_back(edge{ levelIndex, 1 });
		
		levelIndex = m_Level->GetUpperIndex(i);

		if (m_Level->GetTile(levelIndex)->GetState() != TileState::Wall)
			levelEdges[i].push_back(edge{ levelIndex, 1 });
	}


	std::vector<int> minDistance(levelEdges.size(), INT_MAX); //INT_MAX = biggest possible signed int
	minDistance[src] = 0;
	std::set<std::pair<int, int>> activeVerts;
	activeVerts.insert({ 0, src });
	int i = 0;
	while(!activeVerts.empty())
	{
		++i;
		int where = activeVerts.begin()->second;

		m_Path.push_back({ i, where });
		if (where == target)
		{
			return minDistance[where]; //returns the distance
		}
		activeVerts.erase(activeVerts.begin());

		//visited verts are not INT_MAX
		//activeVerts is the possible path
		for (auto ed : levelEdges[where])
			if (minDistance[ed.target] > minDistance[where] + ed.weight) //makes sure i dont check the same tile twice, because non visited is always equal to infinite
			{
				activeVerts.erase({ minDistance[ed.target], ed.target }); //No need for the infinite value anymore
				minDistance[ed.target] = minDistance[where] + ed.weight; //actual walkdistance
				activeVerts.insert({ minDistance[ed.target], ed.target }); //inserting used walkdistance		
			}
	}
	throw std::runtime_error(std::string("getting transform of uninitialized gameObject") + SDL_GetError()); // there is no possible way towards that popint

}

void Ghost::StorePath()
{
	m_ActualPath.clear();
	int road;
	int size = 19;
	road = m_Path[m_Path.size() - 1].second;
	m_ActualPath.push_back(road);
	for (int i{ (int)m_Path.size() - 1 }; i > 0; --i)
	{
		if (m_Path[i].second == road - 1 || m_Path[i].second == road + 1 || m_Path[i].second == road - size || m_Path[i].second == road + size)
		{
			road = m_Path[i].second;
			m_ActualPath.push_back(road);
		}
	}
	std::reverse(m_ActualPath.begin(), m_ActualPath.end());

}

int Ghost::GetTileIndex()
{
	return m_Level->GetActiveTileIndex(shared_from_this());
}

