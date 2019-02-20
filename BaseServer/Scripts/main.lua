require("RoomManager.BaseRoomManager")

local m_BaseRoomManager = nil 

function OnDisconnect(nUid)
	local uid = math.ceil(nUid)
    Log("[Lua] Disconnect:" .. uid)
    m_BaseRoomManager:OnDisconnect(uid)
end

function OnReconnect(nUid)
	local uid = math.ceil(nUid)
    Log("[Lua] Reconnect:" .. uid)
    m_BaseRoomManager:OnReconnect(uid)
end

function OnUpdateUserInfo(nUid,info)
	local uid = math.ceil(nUid)
    Log("[Lua] UpdateUserInfo:" .. uid)
    m_BaseRoomManager:OnUpdateUserInfo(uid,info)
end

function OnMessage( nUid,nCmd,message )
	local uid = math.ceil(nUid)
	local cmd = math.ceil(nCmd)

	Log("[Lua] OnMessage:" .. uid .. ":" .. cmd)
	return m_BaseRoomManager:OnMessage(uid,cmd,message)
end

function Init()
	Log("Init")
    m_BaseRoomManager = BaseRoomManager.new()
    m_BaseRoomManager:Init()
end

--[[
	--c++ part
	UpdatePlaying(uid)
	UpdateExited(uid)
	UpdateUserInfo(uid,info)
	SendData(uid,cmd,message)

	--lua part
	OnMessage( uid,message )
	OnDisconnect(uid)
	OnReconnect(uid)
	OnUpdateUserInfo(uid,info)
]]