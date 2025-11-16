#pragma once

template<unsigned char A, typename T>
class IAction {
public:

	enum EAction : unsigned char {
		NONE = 0,
		STOP_SPEECH,
		USER
	};

	virtual ~IAction() = default;

	static constexpr unsigned char getAction() {
		return A;
	}

	virtual T execute() = 0;
};
