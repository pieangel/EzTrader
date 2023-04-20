#pragma once
#include <string>
#include "SmAccountAsset.h"
namespace DarkHorse {
	/// <summary>
	/// ���� Ŭ����
	/// ���� ������ ������ �ֽ��ϴ�.
	/// </summary>
	class SmAccount
	{
	private:
		/// <summary>
		/// ���� ���̵�
		/// </summary>
		int _id{ -1 };
		/// <summary>
		/// ���� �̸�
		/// </summary>
		std::string _Name;
		/// <summary>
		/// ���� ��ȣ
		/// </summary>
		std::string _No;
		/// <summary>
		/// ���� Ÿ��
		/// </summary>
		std::string _Type;
		/// <summary>
		/// ��� ��ȣ
		/// </summary>
		std::string _pwd;
		
		// 0 : not confirmed, 1 : confirmed.
		int _Confirm{ 0 };
		// used for the fund
		bool _UsedForFund = false;
		int  _SeungSu = 1;
		double _Ratio = 0;
	public:
		SmAccount();
		~SmAccount();
		int Confirm() const { return _Confirm; }
		void Confirm(int val) { _Confirm = val; }
		std::string Name() const { return _Name; }
		void Name(std::string val) { _Name = val; }
		std::string Type() const { return _Type; }
		void Type(std::string val) { _Type = val; }
		std::string No() const { return _No; }
		void No(std::string val) { _No = val; }
		std::string Pwd() const { return _pwd; }
		void Pwd(std::string val) { _pwd = val; }

		SmAccountAsset Asset;
		int id() const { return _id; }
		bool UsedForFund() const { return _UsedForFund; }
		void UsedForFund(bool val) { _UsedForFund = val; }
		int SeungSu() const { return _SeungSu; }
		void SeungSu(int val) { _SeungSu = val; }
		double Ratio() const { return _Ratio; }
		void Ratio(double val) { _Ratio = val; }
	};
}

