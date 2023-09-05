
#pragma once
#include "../Json/json.hpp"
#include "../Common/BlockingCollection.h"
#include "../Common/common.h"
#include <set>

namespace DarkHorse {
	using namespace code_machina;
	const int BulkOutSystemSize = 100;
	class OutSystemProcessor : public Runnable
	{
	public:
		OutSystemProcessor();
		~OutSystemProcessor() override;
		void StartProcess() noexcept;
		void StopProcess() noexcept;
		virtual unsigned int ThreadHandlerProc(void);
		void AddQuote(nlohmann::json&& signal) noexcept;
		bool Enable() const { return _Enable; }
		void Enable(bool val) { _Enable = val; }



	private:
		void ClearTasks();
		bool _Enable{ true };
		bool _BatchProcess{ false };
		BlockingCollection<nlohmann::json> _SignalQueue;
		void ProcessSignal(nlohmann::json&& signal);
		// arr : 데이터가 들어 있는 배열, taken : 실제 데이터가 들어 있는 갯수
		bool ProcessSignal(const std::array<nlohmann::json, BulkOutSystemSize>& arr, const int& taken);
	};
}

