require("Common.class")
require("Commond.Commond")
local json = require("Common.json")
local BaseRoom = require("Room.BaseRoom")

local Room = class(BaseRoom)

function Room:Login(pUser)
    self:super(Room, "Login",pUser);
    --此处添加代码
    --...

	Log("Handler room login")
	strReply = {}
	strReply["code"] = 0
	strReply["roomid"] = self.m_RoomID

	self:Send(pUser,ServerReply.ROOM_LOGIN,json.encode(strReply))

end

function Room:Send( pUser, cmd , str)
	if pUser == nil then
		self:super(Room, "Broadcast",cmd,str)
	else
		self:super(Room, "Unicast",pUser,cmd,str)
	end
end

function Room:Logout(pUser)
    --此处添加代码
    --...

    return self:super(Room, "Logout",pUser);
end

function Room:OnMessage( pUser,cmd,message)
	if self:super(Room, "OnMessage",pUser,cmd,message) then
		return true
	end
	
    if ClientRequest.BULLET_FIRE == cmd then
        self:OnFire(pUse,message)
    elseif ClientRequest.FISH_ATTACK == cmd then
        self:OnFishAttack(pUser,message)
    else
        return false
    end

	return true
end

function Room:OnFire(pUser,jsonMessage)
	self:Send(pUser,ServerReply.BULLET_FIRE_REUSLT,"")
end

function Room:OnFishAttack(pUser,jsonMessage )
	reply = {}
	reply["id"] = jsonMessage["id"];
	self:Send(pUser,ServerReply.FISH_DIE,json.encode(reply))
end

return Room