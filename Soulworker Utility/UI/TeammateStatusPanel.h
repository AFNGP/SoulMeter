#pragma once
#include "pch.h"

#define TEAMSTATUSPANEL TeammateStatusPanel::getInstance()

class TeammateStatusPanel : public Singleton<TeammateStatusPanel> {
private:
	bool _isOpen = false;

	VOID DrawPlayerRow(LPVOID meta, LPVOID player, FLOAT windowWidth);

public:
	VOID OpenWindow();
	VOID Update();
};
