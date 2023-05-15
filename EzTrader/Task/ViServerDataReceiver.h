#pragma once

//#include "../Global/TemplateSingleton.h"
//#include "../EBCtrl/EbGlobal.h"
#include <map>
#include <memory>
#include "../Task/SmTaskDefine.h"
// �������� �����͸� �������� �۾��� �׷캰�� ����ȴ�.
// �ϳ��� �۾� �׷��� ť�� �ְ� �� �۾� �׷��� �ϼ��� �Ǹ� ���� �۾� �׷��� �����Ѵ�.
// �۾� �׷��� ����Ǵ� ������ ���α׷����ٸ� ���ؼ� ǥ�õȴ�.
class VtProgressDlg;

namespace DarkHorse {
	class ViServerDataReceiver 
	{
	private:
		// ���� ���¸� ��Ÿ����.
		//SmAppState _AppState = SmAppState::None;
		// Key : id, value : argument
		//std::map<int, DhTaskArg> 	_RequestMap;
		void execute_request(DhTaskArg& arg);
		// ���� ��Ȳ�� ���³��� ��ȭ����
		//VtProgressDlg* _ProgressDlg;
		std::shared_ptr<VtProgressDlg> progress_dialog_;
		// ���� �۾� ���� �۾��� ������ ����Ѵ�.
		DhTaskInfo task_info_;
		// ���� �۾� ���¸� ǥ���Ѵ�.
		void set_task_state();
		// ���� �׷� �۾��� �����Ѵ�.
		void do_next_group_task();
	public:
		// ť�� ���� �ִ� �۾��� �� ������ �۾��� ���� �մϴ�.
		// ť�� ��� ������ �ƹ��ϵ� ���� �ʽ��ϴ�.
		void execute_next();
		DhTaskArg* find_task(const int& argument_id);
		void on_task_complete(const int& argument_id);
		void on_task_error(const int& argument_id);
		// �������� ���� �������� ������ �����Ų��.
		void end_all_task();
		static int argument_id_;
		static int get_argument_id() {
			return argument_id_++;
		}
		std::shared_ptr<VtProgressDlg> progress_dialog() const { return progress_dialog_; }
		void progress_dialog(std::shared_ptr<VtProgressDlg> val) { progress_dialog_ = val; }

		void start_dm_symbol_master_file_download();
		void start_ab_symbol_master_file_download();
		void start_ab_symbol_master();
		void start_dm_account_asset();
		void start_ab_account_asset();
		void start_ab_account_profit_loss();
		void start_dm_account_profit_loss();
		void start_dm_accepted_order();
		void start_ab_accepted_order();
		void start_ab_symbol_position();
		void start_dm_symbol_position();
		void start_ab_symbol_profit_loss();
		void start_dm_symbol_profit_loss();
	private:
		void make_ab_symbol_profit_loss();
		void make_dm_symbol_profit_loss();
		void make_ab_symbol_position();
		void make_dm_symbol_position();
		void make_dm_accepted_order();
		void make_ab_accepted_order();
		void make_dm_account_asset();
		void make_ab_account_asset();
		void make_ab_symbol_master();
		void make_dm_file_download();
		void make_ab_file_download();
		// �Ϻ� ������ �����ɴϴ�.
		void make_ab_account_profit_loss();
		void make_dm_account_profit_loss();
	};
}
