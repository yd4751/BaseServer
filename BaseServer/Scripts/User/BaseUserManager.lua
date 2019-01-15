require("Common.class")
local ClassUser = require("Local.User")

local BaseUserManager = class()

function BaseUserManager:ctor()
    self.m_Users = {}
end

function BaseUserManager:Add(nUid)
    if self:Get(nUid) ~= nil then
       return 
    end

    self.m_Users[nUid] = ClassUser.new()
    self.m_Users[nUid]:Init(nUid)
    UpdatePlaying(nUid)
end

function BaseUserManager:Remove(nUid)
    if self:Get(nUid) == nil then
       return 
    end

    Log("Remove User:" .. nUid)
    self.m_Users[nUid] = nil
    UpdateExited(nUid)
end

function BaseUserManager:Get(nUid)
    return self.m_Users[nUid]
end

return BaseUserManager
