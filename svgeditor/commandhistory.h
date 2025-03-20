#ifndef COMMANDHISTORY_H_
#define COMMANDHISTORY_H_

#include <memory>
#include <vector>

#include "command.h"

enum class Number { singular, several };

class CommandHistory
{
public:
	static CommandHistory& getInstance();
	~CommandHistory() = default;
	void addCommand(std::shared_ptr<Command> command);
	void addCommands(std::vector<std::shared_ptr<Command>> dids);
	void clearAfterIndex();
	void clearAll();
	void undo();
	void redo();
private:
	CommandHistory();
	CommandHistory(CommandHistory&) = delete;
	CommandHistory(CommandHistory&&) = delete;
	CommandHistory& operator=(const CommandHistory&) = delete;
	CommandHistory& operator=(CommandHistory&&) = delete;
	std::vector<std::shared_ptr<Command>> m_did;
	long long m_index;
	std::vector<size_t> m_num;
	long long m_numindex;
};
#endif // !COMMANDHISTORY_H_

