require("Common.class")
local json = require("Common.json")
require("Config.ConfigRoomManager")
require("Commond.Commond")

local ClassRoom = require("Local.Room")

local ClassUserManger = require("User.BaseUserManager")

BaseRoomManager = class()

function BaseRoomManager:ctor()
    self.m_UserManager = ClassUserManger.new()
    self.m_Rooms = {}
end

function BaseRoomManager:OnDisconnect(uid)
    local pUser = self.m_UserManager:Get(uid)
    if pUsr == nil then
        return 
    end

    local nRoomID = pUser:GetRoomID()
    if nRoomID == nil then
        return 
    end

    self.m_Rooms[nRoomID]:OnDisconnect(pUser)
end

function BaseRoomManager:OnReconnect(uid)
    local pUser = self.m_UserManager:Get(uid)
    if pUsr == nil then
        return 
    end

    local nRoomID = pUser:GetRoomID()
    if nRoomID == nil then
        return 
    end

    self.m_Rooms[nRoomID]:OnReconnect(pUser)
end

function BaseRoomManager:OnUpdateUserInfo(uid,info)
    local pUser = self.m_UserManager:Get(uid)
    if pUsr == nil then
        return 
    end

    local nRoomID = pUser:GetRoomID()
    if nRoomID == nil then
        return 
    end

    self.m_Rooms[nRoomID]:OnUpdateUserInfo(pUser,info)
end

function BaseRoomManager:OnMessage( uid,cmd,message)

    local pUser = self.m_UserManager:Get(uid)
    if pUser == nil then
       if ClientRequest.ROOM_LOGIN == cmd then
            self.m_UserManager:Add(uid)
       else
            Log("Error")
            return false
       end
    end

    pUser = self.m_UserManager:Get(uid)
    if ClientRequest.ROOM_LOGOUT == cmd then
        if pUser:GetRoomID() == nil then
            self.m_UserManager:Remove(pUser:GetID())
            return true
        end
    end

    --获取房间id
    local pRoom = self:FindRoom(pUser)
    if pRoom == nil then
        Log("Find room error")
        self.m_UserManager:Remove(pUser:GetID())
        return false
    end

    local jsonMessage = nil 
    if message ~= nil then 
        jsonMessage = json.decode(message)
    end

    local bRet = pRoom:OnMessage( pUser,cmd,jsonMessage)
    if ClientRequest.ROOM_LOGOUT == cmd then
        if bRet then
            self.m_UserManager:Remove(pUser:GetID())
        end
        return true
    end

    return bRet
end

function BaseRoomManager:Init()
    for roomID = ConfigRoomManager.RoomBegin , ConfigRoomManager.RoomEnd, 1 do
        self.m_Rooms[roomID] = ClassRoom.new()
        self.m_Rooms[roomID]:Init(roomID)
    end
end

---
function BaseRoomManager:FindRoom(pUser)
    local nRoomID = pUser:GetRoomID()
    if nRoomID ~= nil then
        return self.m_Rooms[nRoomID]
    end

    --找到一个空闲桌子
    for roomid , pRoom in pairs(self.m_Rooms) do
        if pRoom:HavePosition() then
            nRoomID = pRoom:GetID()
            break
        end
    end

    if nRoomID ~= nil then
        return self.m_Rooms[nRoomID]
    end

    Log("can not find any valid room!")
    return nil
end

return BaseRoomManager