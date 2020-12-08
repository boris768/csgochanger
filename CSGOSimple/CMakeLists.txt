cmake_minimum_required(VERSION 3.6)
project(SkinChanger)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -static -O2")

set(SOURCE_FILES
        ImGUI/DX9/font_resource.h
        ImGUI/DX9/imgui_impl_dx9.cpp
        ImGUI/DX9/imgui_impl_dx9.h
        ImGUI/imconfig.h
        ImGUI/imgui.cpp
        ImGUI/imgui.h
        ImGUI/imgui_draw.cpp
        ImGUI/imgui_internal.h
        ImGUI/STB_RECT_PACK.H
        ImGUI/stb_textedit.h
        ImGUI/stb_truetype.h
        SourceEngine/CGlobalVarsBase.hpp
        SourceEngine/CHandle.hpp
        SourceEngine/CInput.cpp
        SourceEngine/CInput.hpp
        SourceEngine/ClientClass.hpp
        SourceEngine/Color.hpp
        SourceEngine/Convar.cpp
        SourceEngine/Convar.hpp
        SourceEngine/CRC.cpp
        SourceEngine/CRC.hpp
        SourceEngine/Definitions.hpp
        SourceEngine/IAppSystem.hpp
        SourceEngine/IBaseClientDll.hpp
        SourceEngine/IClientEntity.hpp
        SourceEngine/IClientEntityList.hpp
        SourceEngine/IClientMode.hpp
        SourceEngine/IClientNetworkable.hpp
        SourceEngine/IClientRenderable.hpp
        SourceEngine/IClientThinkable.hpp
        SourceEngine/IClientUnknown.hpp
        SourceEngine/IConVar.hpp
        SourceEngine/ICvar.hpp
        SourceEngine/IEngineTrace.hpp
        SourceEngine/IHandleEntity.hpp
        SourceEngine/IPanel.hpp
        SourceEngine/ISurface.hpp
        SourceEngine/IVEngineClient.hpp
        SourceEngine/PlayerInfo.hpp
        SourceEngine/QAngle.hpp
        SourceEngine/Recv.hpp
        SourceEngine/SDK.cpp
        SourceEngine/SDK.hpp
        SourceEngine/UtlMemory.hpp
        SourceEngine/UtlString.cpp
        SourceEngine/UtlString.hpp
        SourceEngine/UtlVector.hpp
        SourceEngine/Vector.cpp
        SourceEngine/Vector.hpp
        SourceEngine/Vector2D.cpp
        SourceEngine/Vector2D.hpp
        SourceEngine/Vector4D.cpp
        SourceEngine/Vector4D.hpp
        SourceEngine/VMatrix.cpp
        SourceEngine/VMatrix.hpp
        CClientState.h
        CEconWearable.h
        CMaterialReference.cpp
        CMaterialReference.h
        CMemAlloc.h
        Config.cpp
        Config.h
        CPlayerResource.hpp
        CSGOStructs.cpp
        CSGOStructs.hpp
        DrawManager.cpp
        DrawManager.hpp
        Hooks.cpp
        Hooks.hpp
        IBaseViewModel.cpp
        IBaseViewModel.h
        IBaseWorldModel.cpp
        IBaseWorldModel.h
        IFileSystem.h
        IGameMovement.h
        ILocalize.h
        IMaterial.h
        IMaterialSystem.h
        IMoveHelper.h
        IPhysicsSurfaceProps.h
        IPrediction.h
        IStudioRender.h
        ISurface.cpp
        IVModelInfo.h
        json-forwards.h
        json.h
        jsoncpp.cpp
        KeyValues.cpp
        KeyValues.h
        Main.cpp
        Menu.cpp
        Menu.h
        minimal_UtlMemory.h
        NetvarManager.cpp
        NetvarManager.hpp
        Options.cpp
        Options.hpp
        SkinChanger.cpp
        Skins.cpp
        Skins.h
        Stickers.cpp
        Stickers.h
        Utils.hpp
        UtlMemory.cpp
        VFTableHook.hpp
        LoggingSystem.cpp
        XorStr.hpp
        XorStrW.h)
include_directories("e:\\DIrectX\\Include\\")
link_directories("e:\\DIrectX\\Lib\\x86\\")
SET (BUILD_SHARED_LIBS TRUE)
#set_target_properties (SkinChanger PROPERTIES DEFINE_SYMBOL  "COMPILING_DLL" )
#add_executable(CSGOSimple ${SOURCE_FILES})
add_library(SkinChanger SHARED ${SOURCE_FILES})
target_link_libraries(SkinChanger psapi)
target_link_libraries(SkinChanger d3dx9)
target_link_libraries(SkinChanger d3d9)