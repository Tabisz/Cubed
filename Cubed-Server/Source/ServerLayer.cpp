#include "ServerLayer.h"
#include <Walnut/Core/Log.h>

void Cubed::ServerLayer::OnAttach()
{
	m_Console.SetMessageSendCallback([this](std::string_view message) {OnConsoleMessage(message); });
	
	m_Server.SetClientConnectedCallback([this](const Walnut::ClientInfo& clientInfo) {OnClientConnected(clientInfo); });
	m_Server.SetClientDisconnectedCallback([this](const Walnut::ClientInfo& clientInfo) {OnClientDisconnected(clientInfo); });
	m_Server.SetDataReceivedCallback([this](const Walnut::ClientInfo& clientInfo, const Walnut::Buffer buffer) {OnDataReceived(clientInfo, buffer); });

	m_Server.Start();
}

void Cubed::ServerLayer::OnDetach()
{
	m_Server.Stop();
}

void Cubed::ServerLayer::OnUpdate(float ts)
{
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(5ms);
}

void Cubed::ServerLayer::OnUIRender()
{

}

void Cubed::ServerLayer::OnConsoleMessage(std::string_view message)
{
	if (message.starts_with('/'))
	{
		std::cout << "You called the " << message << " command!\n";
	}
}

void Cubed::ServerLayer::OnClientConnected(const Walnut::ClientInfo& clientInfo)
{
	WL_INFO_TAG("Server", "Client connected! ID={}", clientInfo.ID);
}

void Cubed::ServerLayer::OnClientDisconnected(const Walnut::ClientInfo& clientInfo)
{
	WL_INFO_TAG("Server", "Client disconnected! ID={}", clientInfo.ID);

}

void Cubed::ServerLayer::OnDataReceived(const Walnut::ClientInfo& clientInfo, const Walnut::Buffer buffer)
{

}
