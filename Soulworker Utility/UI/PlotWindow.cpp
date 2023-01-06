#include "pch.h"
#include "PlotWindow.h"
#include "UtillWindow.h"
#include <vector>

#include ".\Damage Meter\MySQLite.h"
#include ".\Damage Meter\Damage Meter.h"

VOID PlotWindow::AddData(UINT32 id, string name, DOUBLE DPS, DOUBLE time, bool isFirstElement)
{
	if (_historyMode)
		return;
	if (_pi == nullptr)
		_pi = new PlotInfo();

	_pi->AddData(id, name, DPS, time, isFirstElement);
}

VOID PlotInfo::AddData(UINT32 id, string name, DOUBLE DPS, DOUBLE time, bool isFirstElement)
{
	if (_isHistoryMode)
		return;

	if (isFirstElement) {
		if (_lastTime == time) {
			_allowed = false;
			return;
		}
		else {
			_allowed = true;
		}
	}
	// 
	else {
		if (!_allowed) {
			return;
		}
	}
	_lastTime = time;

	auto it = _metaInfos.begin();
	bool metaInfoFound = false;
	for (; it != _metaInfos.end(); it++) {
		if ((*it)->_id == id) {
			metaInfoFound = true;
			break;
		}
	}

	if (!metaInfoFound) {
		_metaInfos.push_back(new metaInfo(id, name));

		vector<double> newDPSvector;
		newDPSvector.push_back(DPS);
		_dpsList.emplace(id, newDPSvector);

		vector<double> newTimevector;
		newTimevector.push_back(time);
		_timeList.emplace(id, newTimevector);
	}
	else {
		_dpsList[id].push_back(DPS);
		_timeList[id].push_back(time);
	}
}

VOID PlotWindow::AddAbData(DOUBLE DPS, DOUBLE time)
{
	if (_historyMode)
		return;
	if (_pi == nullptr)
		_pi = new PlotInfo();
	_pi->AddAbData(DPS, time);
}

VOID PlotInfo::AddAbData(DOUBLE DPS, DOUBLE time)
{
	if (_isHistoryMode)
		return;

	if (_abLastTime == time) {
		return;
	}
	_abLastTime = time;

	_abList.push_back(DPS);
	_abTimeList.push_back(time);
}

VOID PlotWindow::AddBdData(DOUBLE DPS, DOUBLE time)
{
	if (_historyMode)
		return;
	if (_pi == nullptr)
		_pi = new PlotInfo();
	_pi->AddBdData(DPS, time);
}

VOID PlotInfo::AddBdData(DOUBLE DPS, DOUBLE time)
{
	if (_isHistoryMode)
		return;

	if (_bdLastTime == time) {
		return;
	}
	_bdLastTime = time;

	_bdList.push_back(DPS);
	_bdTimeList.push_back(time);
}

VOID PlotWindow::AddJqData(BYTE stack, DOUBLE time)
{
	if (_historyMode)
		return;
	if (_pi == nullptr)
		_pi = new PlotInfo();
	_pi->AddJqData(stack, time);
}

VOID PlotInfo::AddJqData(BYTE stack, DOUBLE time)
{
	if (_isHistoryMode)
		return;

	if (_jqLastTime == time) {
		return;
	}
	_jqLastTime = time;

	_jqList.push_back(stack);
	_jqTimeList.push_back(time);
}

VOID PlotWindow::AddBossHpData(UINT32 id, UINT64 HP, DOUBLE time)
{
	if (_historyMode)
		return;

	if (_pi == nullptr)
		_pi = new PlotInfo();
	_pi->AddBossHpData(id, HP, time);
}

VOID PlotInfo::AddBossHpData(UINT32 id, UINT64 HP, DOUBLE time)
{
	if (_isHistoryMode)
		return;

	if (_bhLastTime == time) {
		return;
	}
	_bhLastTime = time;

	_bossHpList[id].push_back(static_cast<double>(HP / 1000000));
	_bossTimeList[id].push_back(time);
}

VOID PlotWindow::OpenWindow()
{
	_isOpen = true;
}

VOID PlotWindow::Update()
{
	if (_isOpen) {

		CHAR label[128] = { 0 };
		sprintf_s(label, "%s###MeowGraph", LANGMANAGER.GetText("STR_MENU_MEOW"));

		ImGui::Begin(label, &_isOpen, ImGuiWindowFlags_None);

		if (ImGui::BeginTabBar(u8"PlotWindowTab"))
		{
			if (_pi != nullptr) {
				UpdatePlotTab();
				UpdateAbPlotTab();
				UpdateBdPlotTab();
				UpdateJqPlotTab();
				UpdateBossHpPlotTab();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

VOID PlotWindow::UpdatePlotTab()
{
	if (ImGui::BeginTabItem(LANGMANAGER.GetText("STR_PLOTWINDOW_DPSGRAPH")))
	{
		auto timeList = _pi->GetTimeList();
		auto dpsList = _pi->GetDPSList();
		auto metaInfos = _pi->GetMetaInfo();
		if (timeList.size() > 0) {
			UINT32 firstId = metaInfos.front()->_id;
			UINT32 lastId = metaInfos.back()->_id;
			size_t currentSize = timeList[firstId].size();
			if (currentSize > 0)
			{
				// 
				DOUBLE startX = 0.0;
				if (currentSize > 45) {
					startX = timeList[firstId].at(currentSize - 45);
				}
				DOUBLE endX = timeList[firstId].at(currentSize - 1);
				//
				DOUBLE startY = 0;
				DOUBLE endY = 10000;
				auto itr = dpsList[firstId].begin();
				if (currentSize > 45) {
					itr += (dpsList[firstId].size() - 1) - (45 - 1);
				}
				for (; itr != dpsList[firstId].end(); itr++) {
					if (*itr > endY) {
						endY = *itr;
					}
				}
				startY = endY - 7000;
				if (startY <= 0) {
					startY = 0;
				}
				endY += 1000;

				if (!_end) {
					ImPlot::SetNextPlotLimitsX(startX, endX, ImGuiCond_Always);
					ImPlot::SetNextPlotLimitsY(startY, endY, ImGuiCond_Always);
				}
			}
		}

		if (ImPlot::BeginPlot(
			LANGMANAGER.GetText("STR_PLOTWINDOW_DPSGRAPH"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_TIME_SEC"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_DPSGRAPH"), ImVec2(-1, 0), ImPlotFlags_AntiAliased, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit)) {
			auto it = metaInfos.begin();
			for (; it != metaInfos.end(); it++) {
				UINT32 id = (*it)->_id;
				string name = (*it)->_name;

				ImPlot::PlotLine(name.c_str(), timeList[id].data(), dpsList[id].data(), static_cast<INT>(dpsList[id].size()));
			}

			ImPlot::EndPlot();
		}
		ImGui::EndTabItem();
	}
}

VOID PlotWindow::UpdateAbPlotTab()
{
	if (ImGui::BeginTabItem(LANGMANAGER.GetText("STR_PLOTWINDOW_ABGRAPH")))
	{
		auto _abTimeList = _pi->GetABTimeList();
		auto _abList = _pi->GetABList();
		size_t currentSize = _abTimeList.size();
		if (currentSize > 0)
		{
			DOUBLE startX = 0.0;
			DOUBLE endX = 5.0;
			if (currentSize > 45) {
				startX = _abTimeList.at(currentSize - 45);
			}
			if (currentSize > 0) {
				endX = _abTimeList.back();
			}

			if (!_end) {
				ImPlot::SetNextPlotLimitsX(startX, endX, ImGuiCond_Always);
			}
			ImPlot::SetNextPlotLimitsY(0.0, 100.0, ImGuiCond_Always);
		}
		
		if (ImPlot::BeginPlot(
			LANGMANAGER.GetText("STR_PLOTWINDOW_ABGRAPH"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_TIME_SEC"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_ABGRAPH"), ImVec2(-1, 0), ImPlotFlags_AntiAliased, ImPlotAxisFlags_None, ImPlotAxisFlags_None)) {
			ImPlot::PlotLine(LANGMANAGER.GetText("STR_TABLE_YOU"), _abTimeList.data(), _abList.data(), static_cast<INT>(_abList.size()));
			ImPlot::EndPlot();
		}
		ImGui::EndTabItem();
	}
}

VOID PlotWindow::UpdateBdPlotTab()
{
	if (ImGui::BeginTabItem(LANGMANAGER.GetText("STR_PLOTWINDOW_BDGRAPH")))
	{
		auto _bdTimeList = _pi->GetBDTimeList();
		auto _bdList = _pi->GetBDList();
		size_t currentSize = _bdTimeList.size();
		if (currentSize > 0) {
			// 
			DOUBLE startX = 0.0;
			if (currentSize > 45) {
				startX = _bdTimeList.at(currentSize - 45);
			}
			DOUBLE endX = _bdTimeList.at(currentSize - 1);
			//
			DOUBLE startY = 0;
			DOUBLE endY = 200;
			auto itr = _bdList.begin();
			if (currentSize > 45) {
				itr += (_bdList.size() - 1) - (45 - 1);
			}
			for (; itr != _bdList.end(); itr++) {
				if (*itr > endY) {
					endY = *itr;
				}
			}
			startY = endY - 1000;
			if (startY <= 0) {
				startY = 0;
			}
			endY += 10;

			if (!_end) {
				ImPlot::SetNextPlotLimitsX(startX, endX, ImGuiCond_Always);
				ImPlot::SetNextPlotLimitsY(startY, endY, ImGuiCond_Always);
			}
		}
		if (ImPlot::BeginPlot(
			LANGMANAGER.GetText("STR_PLOTWINDOW_BDGRAPH"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_TIME_SEC"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_BDGRAPH"), ImVec2(-1, 0), ImPlotFlags_AntiAliased, ImPlotAxisFlags_None, ImPlotAxisFlags_None)) {
			ImPlot::PlotLine(LANGMANAGER.GetText("STR_TABLE_YOU"), _bdTimeList.data(), _bdList.data(), static_cast<INT>(_bdList.size()));
			ImPlot::EndPlot();
		}
		ImGui::EndTabItem();
	}
}

VOID PlotWindow::UpdateJqPlotTab()
{
	if (ImGui::BeginTabItem(LANGMANAGER.GetText("STR_PLOTWINDOW_JQGRAPH")))
	{
		auto _jqTimeList = _pi->GetJQTimeList();
		auto _jqList = _pi->GetJQList();
		size_t currentSize = _jqTimeList.size();
		if (currentSize > 0)
		{
			DOUBLE startX = 0.0;
			DOUBLE endX = 5.0;
			if (currentSize > 45) {
				startX = _jqTimeList.at(currentSize - 45);
			}
			if (currentSize > 0) {
				endX = _jqTimeList.back();
			}
			if (!_end) {
				ImPlot::SetNextPlotLimitsX(startX, endX, ImGuiCond_Always);
			}
			ImPlot::SetNextPlotLimitsY(0, 4, ImGuiCond_Always);
		}

		if (ImPlot::BeginPlot(
			LANGMANAGER.GetText("STR_PLOTWINDOW_JQGRAPH"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_TIME_SEC"),
			LANGMANAGER.GetText("STR_PLOTWINDOW_JQGRAPH"), ImVec2(-1, 0), ImPlotFlags_AntiAliased, ImPlotAxisFlags_None, ImPlotAxisFlags_None)) {
			ImPlot::PlotLine(LANGMANAGER.GetText("STR_TABLE_YOU"), _jqTimeList.data(), _jqList.data(), static_cast<INT>(_jqList.size()));
			ImPlot::EndPlot();
		}
		ImGui::EndTabItem();
	}
}

VOID PlotWindow::UpdateBossHpPlotTab()
{
	if (ImGui::BeginTabItem(LANGMANAGER.GetText("STR_PLOTWINDOW_BOSSHPGRAPH")))
	{
		UpdateBossHpPlotCombo();

		if (_selectedBossHpComboID != -1)
			UpdateBossHpPlotGraph();

		ImGui::EndTabItem();
	}
}

VOID PlotWindow::UpdateBossHpPlotCombo()
{
	unordered_map<UINT32, const CHAR*> bossInfos;

	// Get all monster data
	for (auto itr = DAMAGEMETER.begin(); itr < DAMAGEMETER.end(); itr++) {
		for (auto itr2 = (*itr)->begin(); itr2 != (*itr)->end(); itr2++) {
			if ((*itr2)->GetType() == 3 || (*itr2)->GetType() == 4)
				bossInfos.emplace((*itr2)->GetID(), (*itr2)->GetName());
		}
	}

	const CHAR* comboPreview = nullptr;

	if (bossInfos.begin() != bossInfos.end()) {
		if (_selectedBossHpComboID == -1 || bossInfos.find(_selectedBossHpComboID) == bossInfos.end())
			_selectedBossHpComboID = bossInfos.begin()->first;
		comboPreview = bossInfos.at(_selectedBossHpComboID);

		if (ImGui::BeginCombo("BOSS", comboPreview, ImGuiComboFlags_HeightLarge)) {

			for (auto itr = bossInfos.begin(); itr != bossInfos.end(); itr++) 
			{
				CHAR label[MONSTER_NAME_LEN] = { 0 };
				sprintf_s(label, MONSTER_NAME_LEN, "%s##%d", itr->second, itr->first);

				if (ImGui::Selectable(label, _selectedBossHpComboID == itr->first)) {
					_selectedBossHpComboID = itr->first;
				}
			}

			ImGui::EndCombo();
		}
	}
}

VOID PlotWindow::UpdateBossHpPlotGraph()
{
	auto timeList = _pi->GetBossTimeList();
	auto bossHpList = _pi->GetBossHpList();
	if (timeList.size() > 0) {
		size_t currentSize = timeList[_selectedBossHpComboID].size();
		if (currentSize > 0)
		{
			// 
			DOUBLE startX = 0.0;
			if (currentSize > 45) {
				startX = timeList[_selectedBossHpComboID].at(currentSize - 45);
			}
			DOUBLE endX = timeList[_selectedBossHpComboID].at(currentSize - 1);
			//
			DOUBLE startY = 0;
			DOUBLE endY = 100;
			auto itr = bossHpList[_selectedBossHpComboID].begin();
			if (currentSize > 45) {
				itr += (bossHpList[_selectedBossHpComboID].size() - 1) - (45 - 1);
			}
			for (; itr != bossHpList[_selectedBossHpComboID].end(); itr++) {
				if (*itr > endY) {
					endY = *itr;
				}
			}
			startY = endY - 7000;
			if (startY <= 0) {
				startY = 0;
			}
			endY += 100;

			if (!_end) {
				ImPlot::SetNextPlotLimitsX(startX, endX, ImGuiCond_Always);
				ImPlot::SetNextPlotLimitsY(startY, endY, ImGuiCond_Always);
			}
		}
	}

	if (ImPlot::BeginPlot(
		LANGMANAGER.GetText("STR_PLOTWINDOW_BOSSHPGRAPH"),
		LANGMANAGER.GetText("STR_PLOTWINDOW_TIME_SEC"),
		LANGMANAGER.GetText("STR_PLOTWINDOW_BOSSHPGRAPH"), ImVec2(-1, 0), ImPlotFlags_AntiAliased, ImPlotAxisFlags_None, ImPlotAxisFlags_AutoFit)) {
		ImPlot::PlotLine(LANGMANAGER.GetText("STR_PLOTWINDOW_BOSSHPGRAPH_UNIT"), timeList[_selectedBossHpComboID].data(), bossHpList[_selectedBossHpComboID].data(), static_cast<INT>(bossHpList[_selectedBossHpComboID].size()));
		ImPlot::EndPlot();
	}
}

VOID PlotWindow::Start()
{
	_end = false;
}

VOID PlotWindow::End()
{
	_end = true;
}

VOID PlotWindow::Clear()
{
	_end = false;
	_pi = nullptr;
	_selectedBossHpComboID = -1;
	_historyMode = false;
}

VOID PlotWindow::SetPlotInfo(PlotInfo* p_pi)
{
	_pi = p_pi;
	_end = true;
	_historyMode = true;
}

PlotInfo* PlotWindow::GetPlotInfo()
{
	return _pi;
}

PlotWindow::~PlotWindow()
{
}