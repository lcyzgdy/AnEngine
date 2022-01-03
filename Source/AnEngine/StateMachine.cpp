#include "StateMachine.h"
#include "Hash.hpp"
using namespace std;
using namespace AnEngine::Utility;

namespace AnEngine::Game
{
	template<typename _Ty>
	function<bool(_Ty, _Ty)> Cmp[StateMachine::Condition::Count] =
	{
		[](_Ty a, _Ty b) { return a == b; },
		[](_Ty a, _Ty b) { return a != b; },
		[](_Ty a, _Ty b) { return a < b; },
		[](_Ty a, _Ty b) { return a <= b; },
		[](_Ty a, _Ty b) { return a > b; },
		[](_Ty a, _Ty b) { return a >= b; }
	};

	vector<StateMachine*> g_stateMachines;

	void StateMachine::StaticUpdate()
	{
		for (var i : g_stateMachines)
		{
			i->Update();
		}
	}

	void StateMachine::Update()
	{
		m_states[m_curState].Invoke();
		for (var ea : m_states[m_curState].m_events)
		{
			bool f = true;
			for (var item : ea.m_float)
			{
				f = f && Cmp<float>[item.second.second](m_floatParam[item.first], item.second.first);
			}
			for (var item : ea.m_int)
			{
				f = f && Cmp<int>[item.second.second](m_intParam[item.first], item.second.first);
			}
			for (var item : ea.m_bool)
			{
				f = f && Cmp<bool>[item.second.second](m_boolParam[item.first], item.second.first);
			}
			for (var item : ea.m_trigger)
			{
				f = f && m_triggerParam[item];
			}
			if (f)
			{
				m_curState = ea.m_transState;

				for (var item : ea.m_trigger)
				{
					m_triggerParam[item] = false;
				}

				break;
			}
		}
	}

	StateMachine::StateMachine()
	{
		g_stateMachines.push_back(this);
	}

	StateMachine::~StateMachine()
	{
		g_stateMachines.erase(find(g_stateMachines.begin(), g_stateMachines.end(), this));
	}

	int StateMachine::GetStateIndex(const std::wstring& name)
	{

		return -1;
	}

	int StateMachine::GetStateIndex(std::wstring&& name)
	{
		return -1;
	}

	std::wstring StateMachine::GetCurrectStateName()
	{
		return m_stateName[m_curState];
	}

	void StateMachine::AddIntParam(wstring&& name, int initValue)
	{
		if (m_str2Hash.find(name) != m_str2Hash.end())
		{
			throw exception();
		}
		uint64_t h = m_str2Hash[name] = m_str2Hash.size();
		m_intParam[h] = initValue;
	}

	void StateMachine::AddFloatParam(wstring&& name, float initValue)
	{
		if (m_str2Hash.find(name) != m_str2Hash.end())
		{
			throw exception();
		}
		uint64_t h = m_str2Hash[name] = m_str2Hash.size();
		m_floatParam[h] = initValue;
	}

	void StateMachine::AddBoolParam(wstring&& name, bool initValue)
	{
		if (m_str2Hash.find(name) != m_str2Hash.end())
		{
			throw exception();
		}
		uint64_t h = m_str2Hash[name] = m_str2Hash.size();
		m_boolParam[h] = initValue;
	}

	void StateMachine::AddTrigerParam(wstring&& name)
	{
		if (m_str2Hash.find(name) != m_str2Hash.end())
		{
			throw exception();
		}
		uint64_t h = m_str2Hash[name] = m_str2Hash.size();
		m_triggerParam[h] = false;
	}

	int StateMachine::CreateNewState(std::wstring && name, const std::function<void()>& func)
	{
		int index = m_stateName.size();
		std::hash<std::wstring> h;
		uint64_t ha = h(name);
		m_stateName.emplace_back(name);
		m_states.emplace_back(State(ha, func));
		return index;
	}

	int StateMachine::CreateNewState(std::wstring && name, std::function<void()>&& func)
	{
		int index = m_stateName.size();
		std::hash<std::wstring> h;
		uint64_t ha = h(name);
		m_stateName.emplace_back(name);
		m_states.emplace_back(State(ha, func));
		return index;
	}

	void StateMachine::CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, uint32_t newValue, Condition cond)
	{
		for (var i : m_states[from].m_events)
		{
			if (i.m_transState == to)
			{
				i.m_int[m_str2Hash[paramName]] = { newValue, cond };
				return;
			}
		}
		Event ev(to);
		ev.m_int[m_str2Hash[paramName]] = { newValue, cond };
		m_states[from].m_events.emplace_back(move(ev));
	}

	void StateMachine::CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, float newValue, Condition cond)
	{
		for (var i : m_states[from].m_events)
		{
			if (i.m_transState == to)
			{
				i.m_float[m_str2Hash[paramName]] = { newValue, cond };
				return;
			}
		}
		Event ev(to);
		ev.m_float[m_str2Hash[paramName]] = { newValue, cond };
		m_states[from].m_events.emplace_back(move(ev));
	}

	void StateMachine::CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, bool newValue, Condition cond)
	{
		for (var i : m_states[from].m_events)
		{
			if (i.m_transState == to)
			{
				i.m_bool[m_str2Hash[paramName]] = { newValue, cond };
				return;
			}
		}
		Event ev(to);
		ev.m_bool[m_str2Hash[paramName]] = { newValue, cond };
		m_states[from].m_events.emplace_back(move(ev));
	}

	void StateMachine::CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& tiggerName)
	{
		for (var i : m_states[from].m_events)
		{
			if (i.m_transState == to)
			{
				i.m_trigger.emplace_back(m_str2Hash[tiggerName]);
				return;
			}
		}
		Event ev(to);
		ev.m_trigger.emplace_back(m_str2Hash[tiggerName]);
		m_states[from].m_events.emplace_back(move(ev));
	}

	void StateMachine::SetInt(std::wstring&& name, int value)
	{
		m_intParam[m_str2Hash[name]] = value;
	}

	void StateMachine::SetBool(std::wstring&& name, bool value)
	{
		m_boolParam[m_str2Hash[name]] = value;
	}

	void StateMachine::SetFloat(std::wstring&& name, float value)
	{
		m_floatParam[m_str2Hash[name]] = value;
	}

	void StateMachine::SetTrigger(std::wstring&& name)
	{
		m_triggerParam[m_str2Hash[name]] = true;
	}

	void StateMachine::SetCurrentState(int index)
	{
		if (index < 0 || index >= m_states.size())
		{
			throw exception();
		}
		m_curState = index;
	}
}