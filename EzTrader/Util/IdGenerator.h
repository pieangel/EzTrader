#pragma once
namespace DarkHorse {
class IdGenerator
{
public:
	// Static Members
	static int _id;
	static int get_id() { return _id++; }
};
}

