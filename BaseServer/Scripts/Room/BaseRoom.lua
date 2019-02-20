require("Common.class")
require("Commond.Commond")

local ClassUser require("User.BaseUser")

local BaseRoom = class()

function BaseRoom:ctor()
    self.m_GameUser = {}
    self.m_RoomID = nil
end

function BaseRoom:Init(nRoomID)
    self.m_RoomID = nRoomID
    Log("Room init:" .. self.m_RoomID)
end

function BaseRoom:HavePosition()
    return true
end

function BaseRoom:GetID()
    return self.m_RoomID
end
--json 已解析
function BaseRoom:OnMessage( pUser,cmd,message)
    if ClientRequest.ROOM_LOGIN == cmd then
        self:Login(pUser)
    elseif ClientRequest.ROOM_LOGOUT == cmd then
        return self:Logout(pUser)
    else
        return false
    end

    return true
end

function BaseRoom:OnDisconnect(pUser)
end

function BaseRoom:OnReconnect(pUser)
end

function BaseRoom:OnUpdateUserInfo(pUser,info)
    pUser:OnUpdateUserInfo(info)
end

--
function BaseRoom:Login(pUser)
    self.m_GameUser[pUser:GetID()] = pUser
    self.m_GameUser[pUser:GetID()]:BindRoom(self.m_RoomID)
end
function BaseRoom:Logout(pUser)
    local bRet = true

    if bRet then
        pUser:DisbindRoom()
        self.m_GameUser[pUser:GetID()] = nil
    end

    return bRet
end

function BaseRoom:Unicast(pUser,cmd,strSend)
    pUser:Send(cmd,strSend)
end

function BaseRoom:Broadcast(cmd,strSend)
    for uid , pUser in pairs(self.m_GameUser) do
        pUser:Send(cmd,strSend)
    end
end

return BaseRoom