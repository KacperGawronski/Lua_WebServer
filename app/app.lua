function process_request(http_request)
	local function main()
		local GET_value=string.gsub(http_request,"GET (.-) HTTP/1%.1.*","%1")
		local GET_response_header="HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: close\r\n\r\n"
		coroutine.yield(GET_response_header)
		coroutine.yield("<html><body>")
		coroutine.yield("Hello World<br/>")
		coroutine.yield(string.format("GET REQUEST: %s</br>",GET_value))
		coroutine.yield("TEST OK<br/>")
		coroutine.yield("</body></html>\r\n")
	end
	return main
end
