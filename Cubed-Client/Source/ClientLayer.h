#pragma once

#include "Walnut/Application.h"
#include "Walnut/Layer.h"

#include "Walnut/Networking/Client.h"

#include <glm/glm.hpp>
namespace Cubed
{
	class ClientLayer : public Walnut::Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float ts) override;
		virtual void OnUIRender() override;
	private:
		void OnDataReceived(const Walnut::Buffer buffer);
	private:
		glm::vec2 m_PlayerPosition{ 50,50 };
		glm::vec2 m_PlayerVelocity{ 0, 0 };

		std::string m_ServerAddress;

		Walnut::Client m_Client;

		uint32_t m_PlayerID = 0;
	};

}