
// ChestWindow.h

// Representing the UI window for the chest block





#pragma once

#include "Window.h"
#include "../BlockEntities/ChestEntity.h"
#include "../Entities/Player.h"





class cChestWindow :
	public cWindow
{
	typedef cWindow super;

public:
	cChestWindow(cChestEntity * a_Chest) :
		cWindow(wtChest, (a_Chest->GetBlockType() == E_BLOCK_CHEST) ? "Chest" : "Trapped Chest"),
		m_World(a_Chest->GetWorld()),
		m_BlockX(a_Chest->GetPosX()),
		m_BlockY(a_Chest->GetPosY()),
		m_BlockZ(a_Chest->GetPosZ()),
		m_PrimaryChest(a_Chest),
		m_SecondaryChest(nullptr)
	{
		m_SlotAreas.push_back(new cSlotAreaChest(a_Chest, *this));
		m_SlotAreas.push_back(new cSlotAreaInventory(*this));
		m_SlotAreas.push_back(new cSlotAreaHotBar(*this));

		// Play the opening sound:
		m_World->BroadcastSoundEffect("random.chestopen", (double)m_BlockX, (double)m_BlockY, (double)m_BlockZ, 1, 1);

		// Send out the chest-open packet:
		m_World->BroadcastBlockAction(m_BlockX, m_BlockY, m_BlockZ, 1, 1, a_Chest->GetBlockType());
	}

	cChestWindow(cChestEntity * a_PrimaryChest, cChestEntity * a_SecondaryChest) :
		cWindow(wtChest, (a_PrimaryChest->GetBlockType() == E_BLOCK_CHEST) ? "Double Chest" : "Double Trapped Chest"),
		m_World(a_PrimaryChest->GetWorld()),
		m_BlockX(a_PrimaryChest->GetPosX()),
		m_BlockY(a_PrimaryChest->GetPosY()),
		m_BlockZ(a_PrimaryChest->GetPosZ()),
		m_PrimaryChest(a_PrimaryChest),
		m_SecondaryChest(a_SecondaryChest)
	{
		m_SlotAreas.push_back(new cSlotAreaDoubleChest(a_PrimaryChest, a_SecondaryChest, *this));
		m_SlotAreas.push_back(new cSlotAreaInventory(*this));
		m_SlotAreas.push_back(new cSlotAreaHotBar(*this));

		m_ShouldDistributeToHotbarFirst = false;

		// Play the opening sound:
		m_World->BroadcastSoundEffect("random.chestopen", (double)m_BlockX, (double)m_BlockY, (double)m_BlockZ, 1, 1);

		// Send out the chest-open packet:
		m_World->BroadcastBlockAction(m_BlockX, m_BlockY, m_BlockZ, 1, 1, a_PrimaryChest->GetBlockType());
	}

	~cChestWindow()
	{
		// Send out the chest-close packet:
		m_World->BroadcastBlockAction(m_BlockX, m_BlockY, m_BlockZ, 1, 0, m_PrimaryChest->GetBlockType());

		m_World->BroadcastSoundEffect("random.chestclosed", (double)m_BlockX, (double)m_BlockY, (double)m_BlockZ, 1, 1);
	}

	virtual bool ClosedByPlayer(cPlayer & a_Player, bool a_CanRefuse) override
	{
		int ChunkX, ChunkZ;

		m_PrimaryChest->SetNumberOfPlayers(m_PrimaryChest->GetNumberOfPlayers() - 1);
		cChunkDef::BlockToChunk(m_PrimaryChest->GetPosX(), m_PrimaryChest->GetPosZ(), ChunkX, ChunkZ);
		m_PrimaryChest->GetWorld()->MarkRedstoneDirty(ChunkX, ChunkZ);

		if (m_SecondaryChest != nullptr)
		{
			m_SecondaryChest->SetNumberOfPlayers(m_SecondaryChest->GetNumberOfPlayers() - 1);
			cChunkDef::BlockToChunk(m_SecondaryChest->GetPosX(), m_SecondaryChest->GetPosZ(), ChunkX, ChunkZ);
			m_SecondaryChest->GetWorld()->MarkRedstoneDirty(ChunkX, ChunkZ);
		}

		cWindow::ClosedByPlayer(a_Player, a_CanRefuse);
		return true;
	}

	virtual void OpenedByPlayer(cPlayer & a_Player) override
	{
		int ChunkX, ChunkZ;

		m_PrimaryChest->SetNumberOfPlayers(m_PrimaryChest->GetNumberOfPlayers() + 1);
		cChunkDef::BlockToChunk(m_PrimaryChest->GetPosX(), m_PrimaryChest->GetPosZ(), ChunkX, ChunkZ);
		m_PrimaryChest->GetWorld()->MarkRedstoneDirty(ChunkX, ChunkZ);

		if (m_SecondaryChest != nullptr)
		{
			m_SecondaryChest->SetNumberOfPlayers(m_SecondaryChest->GetNumberOfPlayers() + 1);
			cChunkDef::BlockToChunk(m_SecondaryChest->GetPosX(), m_SecondaryChest->GetPosZ(), ChunkX, ChunkZ);
			m_SecondaryChest->GetWorld()->MarkRedstoneDirty(ChunkX, ChunkZ);
		}

		cWindow::OpenedByPlayer(a_Player);
	}
	
protected:
	cWorld * m_World;
	int m_BlockX, m_BlockY, m_BlockZ;  // Position of the chest, for the window-close packet
	cChestEntity * m_PrimaryChest;
	cChestEntity * m_SecondaryChest;
};




