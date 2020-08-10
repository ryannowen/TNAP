#pragma once

#include "ImGuiInclude.hpp"
#include "LoadModelMessage.hpp"
#include "LoadTextureMessage.hpp"

namespace TNAP {

	class IMGUILoadBase
	{
	protected:
		bool m_shouldClose{ false };

	public:
		IMGUILoadBase()
		{}

		inline const bool getShouldClose() const { return m_shouldClose; }
		inline virtual void setFilePath(const std::string& argFilePath) = 0;
		virtual void ImGuiRenderLoad(const size_t argLoadWindowID) = 0;
	};

	class IMGUIModelLoad : public IMGUILoadBase
	{
	private:
		LoadModelMessage m_loadMessage;

	public:
		IMGUIModelLoad()
			: IMGUILoadBase()
		{}

		inline virtual void setFilePath(const std::string& argFilePath) { m_loadMessage.m_modelFilepath = argFilePath; }
		virtual void ImGuiRenderLoad(const size_t argLoadWindowID) override final;
	};

	class IMGUITextureLoad : public IMGUILoadBase
	{
		LoadTextureMessage m_loadMessage;

	public:
		IMGUITextureLoad()
			: IMGUILoadBase()
		{}

		inline virtual void setFilePath(const std::string& argFilePath) { m_loadMessage.m_loadInfo.second = argFilePath; }
		virtual void ImGuiRenderLoad(const size_t argLoadWindowID) override final;
	};
}
