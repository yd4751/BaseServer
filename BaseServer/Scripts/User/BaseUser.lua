require("Common.class")

local BaseUser = class()

function BaseUser:ctor()
end

function BaseUser:Init(uid)
    self.m_uid = uid
    Log("New User:" .. self.m_uid)
end

function BaseUser:Update(info)
    Log("User update")
end

function BaseUser:BindRoom(roomid)
    self.m_roomid = roomid
    Log("BindRoom:" .. roomid)
end

function BaseUser:DisbindRoom()
    self.m_roomid = nil
    Log("DisbindRoom")
end

function BaseUser:GetRoomID()
    return self.m_roomid
end

function BaseUser:GetID()
    return self.m_uid
end

function BaseUser:Send(cmd,strSend)
    SendData(self.m_uid,cmd,strSend)
end

return BaseUser