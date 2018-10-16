function process_http_request(http_request)
	local function gen()
		local GET_value=string.gsub(http_request,"GET (.-) HTTP/1%.1.*","%1")
		local GET_response_header="HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n"
		coroutine.yield(GET_response_header)
		coroutine.yield(string.format("<html><body>Hello World<br/>%s</body></html>\r\n",GET_value))
		coroutine.yield('')
	end
	coroutine.wrap(function () gen() end)
end
