#include "ClientLayer.h"

#include "Walnut/Input/Input.h"
#include "Walnut/ImGui/ImGuiTheme.h"


#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Walnut/Serialization/BufferStream.h"
#include "ServerPacket.h"


namespace Cubed
{
	static Walnut::Buffer s_ScratchBuffer;


	static void DrawRect(glm::vec2 position, glm::vec2 size, uint32_t color)
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImVec2 min = ImGui::GetWindowPos() + ImVec2(position.x, position.y);
		ImVec2 max = min + ImVec2(size.x,size.y);

		drawList->AddRectFilled(min, max, color);
	}
	void ClientLayer::OnAttach()
	{
		s_ScratchBuffer.Allocate(10 * 1024 * 1024);


		m_Client.SetDataReceivedCallback([this](const Walnut::Buffer buffer) {OnDataReceived(buffer); });
	}
	void ClientLayer::OnDetach()
	{

	}
	void ClientLayer::OnUpdate(float ts)
	{
		Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		if (connectionStatus != Walnut::Client::ConnectionStatus::Connected)
			return;

		glm::vec2 dir{ 0.0f, 0.0f };
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
			dir.y = -1;
		else if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
			dir.y = 1;

		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
			dir.x = -1;
		if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
			dir.x = 1;

		if (glm::length(dir) > 0.0f)
		{
			dir = glm::normalize(dir);
			const float speed = 50.0f;
			m_PlayerVelocity = dir * speed;
		}
		
		m_PlayerVelocity = glm::mix(m_PlayerVelocity, glm::vec2(0.0f), 2 * ts);

		m_PlayerPosition += m_PlayerVelocity * ts;

		Walnut::BufferStreamWriter stream(s_ScratchBuffer);

		stream.WriteRaw(PacketType::ClientUpdate);
		stream.WriteRaw<glm::vec2>(m_PlayerPosition);
		stream.WriteRaw<glm::vec2>(m_PlayerVelocity);
		m_Client.SendBuffer(stream.GetBuffer());
	}
	void ClientLayer::OnUIRender()
	{
		Walnut::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		if (connectionStatus == Walnut::Client::ConnectionStatus::Connected)
		{
			DrawRect(m_PlayerPosition, { 200, 200 }, 0xffff00ff);
		}
		else
		{
			ImGui::Begin("Connect to server");
				

			ImGui::InputText("Server address", &m_ServerAddress);
			if(connectionStatus == Walnut::Client::ConnectionStatus::FailedToConnect)
				ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::invalidPrefab),"Failed to connect.");
			else if (connectionStatus == Walnut::Client::ConnectionStatus::Connecting)
				ImGui::TextColored(ImColor(Walnut::UI::Colors::Theme::textDarker), "Connecting.");

			if (ImGui::Button("Connect"))
			{
				m_Client.ConnectToServer(m_ServerAddress);
			}



			ImGui::End();
		}

	}
	void ClientLayer::OnDataReceived(const Walnut::Buffer buffer)
	{
		Walnut::BufferStreamReader stream(buffer);
		PacketType type;
		stream.ReadRaw(type);
		switch (type)
		{
		case PacketType::None:
			break;
		case PacketType::Message:
			break;
		case PacketType::ClientConnectionRequest:
			break;
		case PacketType::ConnectionStatus:
			break;
		case PacketType::ClientList:
			break;
		case PacketType::ClientConnect:
			uint32_t idFromServer;
			stream.ReadRaw<uint32_t>(idFromServer);
			m_PlayerID = idFromServer;
			break;

		case PacketType::ClientUpdate:
			break;
		case PacketType::ClientDisconnect:
			break;
		case PacketType::ClientUpdateResponse:
			break;
		case PacketType::MessageHistory:
			break;
		case PacketType::ServerShutdown:
			break;
		case PacketType::ClientKick:
			break;
		default:
			break;
		}
	}
}
