#pragma once
#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include "ObjectBehaviour.h"
#include "onwind.h"

namespace AnEngine::Game
{
	class StateMachine : public ObjectBehaviour
	{
	public:
		enum Condition
		{
			Equal,
			NotEqual,
			Less,
			LessOrEqual,
			Greater,
			GreaterOrEqual,
			Count
		};

		enum ParamType
		{
			Int, Float, Bool, Trigger
		};

		/*template<typename _RtTy, typename... _Ty>
		class State
		{
			uint64_t m_hash;
			int m_id;
			std::function<_RtTy(_Ty...)> m_func;
		public:
			State(uint64_t hash, const std::function<_RtTy(_Ty...)>& func) : m_hash(hash), m_func(func) {}
			State(uint64_t hash, std::function<_RtTy(_Ty...)>&& func) : m_hash(hash), m_func(func) {}
			~State() = default;

			inline _RtTy Invoke(_Ty... t) { return m_func(t...); }
			inline _RtTy ReferenceInvoke(_Ty&... t) { return m_func(t...); }

			bool operator==(const State& rhs) { return this->m_hash == rhs.m_hash; }
		};

		template<typename _RtTy = void>
		class State
		{
			uint64_t m_hash;
			int m_id;
			std::function<_RtTy()> m_func;
		public:
			State(uint64_t hash, const std::function<_RtTy()>& func) : m_hash(hash), m_func(func) {}
			State(uint64_t hash, std::function<_RtTy()>&& func) : m_hash(hash), m_func(func) {}
			~State() = default;

			inline _RtTy Invoke() { return m_func(); }
			inline _RtTy ReferenceInvoke() { return m_func(); }

			bool operator==(const State& rhs) { return this->m_hash == rhs.m_hash; }
		};*/

		struct Event
		{
			uint32_t m_transState;
			std::map<uint64_t, std::pair<int, Condition>> m_int;
			std::map<uint64_t, std::pair<float, Condition>> m_float;
			std::map<uint64_t, std::pair<bool, Condition>> m_bool;
			std::vector<uint64_t> m_trigger;

			bool operator<(const Event& rhs) { return this->m_transState < rhs.m_transState; }

			Event(uint32_t to) : m_transState(to) {}
		};

		struct State
		{
		private:
			friend class StateMachine;

			uint64_t m_id;
			std::function<void()> m_func;
			std::vector<Event> m_events;

		public:
			State(uint64_t hash, const std::function<void()>& func) : m_id(hash), m_func(func) {}
			State(uint64_t hash, std::function<void()>&& func) : m_id(hash), m_func(func) {}
			~State() = default;

			inline void Invoke() { return m_func(); }
			inline void ReferenceInvoke() { return m_func(); }

			bool operator==(const State& rhs) { return this->m_id == rhs.m_id; }
		};

		friend class Scene;
		static void StaticUpdate();

	private:
		std::unordered_map<std::wstring, uint64_t> m_str2Hash;

		std::map<uint64_t, int> m_intParam;
		std::map<uint64_t, float> m_floatParam;
		std::map<uint64_t, bool> m_boolParam;
		// 触发器，使用后触发器状态自动变为0
		std::map<uint64_t, bool> m_triggerParam;

		std::vector<std::wstring> m_stateName;
		std::vector<State> m_states;
		uint32_t m_curState;

	protected:
		//virtual void Start() final;
		virtual void Update() final;

	public:
		StateMachine();
		virtual ~StateMachine();

		int GetStateIndex(const std::wstring& name);
		int GetStateIndex(std::wstring&& name);
		std::wstring GetCurrectStateName();
		// 增添状态机参数
		void AddIntParam(std::wstring&& name, int initValue);
		void AddFloatParam(std::wstring&& name, float initValue);
		void AddBoolParam(std::wstring&& name, bool initValue);
		void AddTrigerParam(std::wstring&& name);
		// 创建新状态
		int CreateNewState(std::wstring&& name, const std::function<void()>& func);
		int CreateNewState(std::wstring&& name, std::function<void()>&& func);
		// 添加状态转移条件
		void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, uint32_t newValue, Condition cond);
		void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, float newValue, Condition cond);
		void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& paramName, bool newValue, Condition cond);
		void CreateStateTransCondition(uint32_t from, uint32_t to, std::wstring&& tiggerName);
		// 外部设置状态机
		void SetInt(std::wstring&& name, int value);
		void SetBool(std::wstring&& name, bool value);
		void SetFloat(std::wstring&& name, float value);
		void SetTrigger(std::wstring&& name);
		void SetCurrentState(int index);
	};
}
#endif // !__STATEMACHINE_H__