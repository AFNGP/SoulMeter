#include "pch.h"
#include ".\UI\TeammateStatusPanel.h"
#include ".\Damage Meter\Damage Meter.h"
#include ".\Soulworker Packet\PacketInfo.h"
#include ".\UI\Option.h"
#include ".\UI\UiWindow.h"
#include ".\UI\PlayerTable.h"

VOID TeammateStatusPanel::OpenWindow() {
	_isOpen = !_isOpen;
}

VOID TeammateStatusPanel::DrawPlayerRow(LPVOID pMeta, LPVOID pPlayer, FLOAT windowWidth) {
	SWDamageMeter::SW_PLAYER_METADATA* meta = (SWDamageMeter::SW_PLAYER_METADATA*)pMeta;
	SWDamagePlayer* player = (SWDamagePlayer*)pPlayer;

	ImU32 jobColor = UIOPTION.GetJobColor(meta->_job);
	ImVec4 jobColorF = ImGui::ColorConvertU32ToFloat4(jobColor);

	// Name + DPS on same line
	FLOAT tableTime = PLAYERTABLE.GetTableTime();
	if (tableTime >= 1.0f) {
		DOUBLE dps = (DOUBLE)player->GetDamage() / tableTime / 1000.0;
		ImGui::TextColored(jobColorF, "%s", meta->_name[0] != '\0' ? meta->_name : "???");
		ImGui::SameLine();
		ImGui::TextDisabled("%.0fK DPS", dps);
	}
	else {
		ImGui::TextColored(jobColorF, "%s", meta->_name[0] != '\0' ? meta->_name : "???");
	}

	FLOAT hp = meta->GetStat(StatType::CurrentHP);
	FLOAT maxHp = meta->GetStat(StatType::MaxHP);

	// HP bar
	if (maxHp > 0.0f) {
		FLOAT hpPct = hp / maxHp;
		if (hpPct < 0.0f) hpPct = 0.0f;
		if (hpPct > 1.0f) hpPct = 1.0f;

		ImVec2 barPos = ImGui::GetCursorScreenPos();
		FLOAT barWidth = windowWidth - ImGui::GetStyle().WindowPadding.x * 2.0f;
		FLOAT barHeight = 8.0f;

		ImDrawList* dl = ImGui::GetWindowDrawList();
		dl->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight), IM_COL32(60, 60, 60, 200));
		dl->AddRectFilled(barPos, ImVec2(barPos.x + barWidth * hpPct, barPos.y + barHeight),
			hpPct > 0.3f ? IM_COL32(50, 200, 50, 220) : IM_COL32(220, 50, 50, 220));
		ImGui::Dummy(ImVec2(barWidth, barHeight));

		CHAR hpText[64];
		sprintf_s(hpText, "HP: %.0f / %.0f (%.0f%%)", hp, maxHp, hpPct * 100.0f);
		ImGui::TextDisabled("%s", hpText);
	}

	// Stats row
	FLOAT ab = meta->GetStat(StatType::ArmorBreak);
	FLOAT bd = meta->GetSpecialStat(SpecialStatType::BossDamageAddRate);
	FLOAT as_ = meta->GetStat(StatType::AttackSpeed);
	FLOAT atk = meta->GetStat(StatType::MaxAttack);

	ImGui::Text("AB:%.1f%%  BD:%.1f%%  AS:%.1f%%  ATK:%.0f",
		ab, bd, as_, atk);

	ImGui::Separator();
}

VOID TeammateStatusPanel::Update() {

	if (!_isOpen)
		return;

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;

	if (!ImGui::Begin("Teammate Status###TeammateStatus", &_isOpen, flags)) {
		ImGui::End();
		return;
	}

	FLOAT windowWidth = ImGui::GetWindowWidth();

	bool anyDrawn = false;
	for (auto itr = DAMAGEMETER.begin(); itr != DAMAGEMETER.end(); ++itr) {
		UINT32 id = (*itr)->GetID();
		SWDamageMeter::SW_PLAYER_METADATA* meta = DAMAGEMETER.GetPlayerMetaData(id);
		if (meta == nullptr)
			continue;
		DrawPlayerRow((LPVOID)meta, (LPVOID)(*itr), windowWidth);
		anyDrawn = true;
	}

	if (!anyDrawn) {
		ImGui::TextDisabled("No party data");
	}

	ImGui::End();
}
