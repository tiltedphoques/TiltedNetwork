#pragma once

#include <Allocator.hpp>
#include <Buffer.hpp>

namespace TiltedPhoques
{
	struct Channel
	{
		enum Type
		{
			kReliable,
			kUnreliable,
			kSequenced
		};

		Channel(Type aType, std::function<void(Buffer*)> aMessageReceivedCallback);
		~Channel();

		TP_NOCOPYMOVE(Channel);
		TP_ALLOCATOR;

		void ProcessPacket(Buffer::Reader* apReader);

	protected:

		void ProcessUnreliablePacket(Buffer::Reader* apReader);
		void ProcessReliablePacket(Buffer::Reader* apReader);
		void ProcessSequencedPacket(Buffer::Reader* apReader);

	private:

		Type m_type;
		std::function<void(Buffer*)> m_messageReceivedCallback;
	};
}