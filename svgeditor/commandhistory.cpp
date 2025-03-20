#include "commandhistory.h"

CommandHistory& CommandHistory::getInstance()
{
	static CommandHistory instance;
	return instance;
}
CommandHistory::CommandHistory()
	: m_index(-1)
	, m_numindex(-1)
{
}
void CommandHistory::addCommand(std::shared_ptr<Command> command)
{
	clearAfterIndex();
	m_did.push_back(command);
	m_num.push_back(1);
	++m_index;
	++m_numindex;
}
void CommandHistory::addCommands(std::vector<std::shared_ptr<Command>> dids)
{
	clearAfterIndex();
	copy(dids.begin(), dids.end(), std::back_inserter(m_did));
	m_num.push_back(dids.size());
	m_index += dids.size();
	++m_numindex;
}
void CommandHistory::clearAfterIndex()
{
	if (m_index + 1 + m_did.begin() != m_did.end())
		m_did.erase(m_did.begin() + 1 + m_index, m_did.end());
	if (m_numindex + 1 + m_num.begin() != m_num.end())
		m_num.erase(m_num.begin() + 1 + m_index, m_num.end());
}
void CommandHistory::clearAll()
{
	m_index = -1;
	m_numindex = -1;
	m_did.clear();
	m_num.clear();
}
void CommandHistory::undo()
{
	if (m_numindex < 0)
		return;
	if (m_num.at(m_numindex) == 1)
	{
		m_did.at(m_index)->undo();
		--m_index;
	}
	else
	{
		auto rbegin = std::make_reverse_iterator(m_did.begin() + 1 + m_index);
		auto rend = rbegin + m_num.at(m_numindex);
		std::for_each(rbegin, rend, [](std::shared_ptr<Command> iter)
			{
				iter->undo();
			});
		m_index -= m_num.at(m_numindex);
	}
	--m_numindex;
}
void CommandHistory::redo()
{
	if (m_num.empty())
		return;
	if (m_num.begin() + 1 + m_numindex == m_num.end())
		return;
	++m_numindex;
	if (m_num.at(m_numindex) == 1)
	{
		++m_index;
		m_did.at(m_index)->redo();
	}
	else
	{
		auto begin = m_did.begin() + 1 + m_index;
		auto end = begin + m_num.at(m_numindex);
		std::for_each(begin, end, [](std::shared_ptr<Command> iter)
			{
				iter->redo();
			});
		m_index += m_num.at(m_numindex);
	}
}