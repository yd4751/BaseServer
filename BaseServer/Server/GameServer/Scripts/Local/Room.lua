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
	
	return true
end


return Room