--请求命令范围 401000 ~ 450000 左右开区间（两边取不到）
--回复命令范围 451000 ~ 500000 左右开区间（两边取不到）
ClientRequest = {
    ROOM_LOGIN = 401001,
	ROOM_LOGOUT = 401002,
	OUT_CARD = 401003,
}

ServerReply = {
    ROOM_LOGIN = 451001,
	ROOM_LOGOUT = 451002,
	GAME_START = 451003,
	SEND_CARD = 451004,
	OUT_CARD_NOTIFY = 451005,
	OUT_CARD_RESULT = 451006,
	GAME_END = 451007,
}