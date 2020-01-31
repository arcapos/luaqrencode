local qrencode = require 'qrencode'

print('qrencode module information:')
print(qrencode._DESCRIPTION)
print(qrencode._COPYRIGHT)
print(qrencode._VERSION)

print('API version string: ' .. qrencode.APIVersionString())

local APIVersion = qrencode.APIVersion()
print(string.format('API version: major=%d, minor=%d, micro=%d',
    APIVersion.major, APIVersion.minor, APIVersion.micro))

local version, width, data = qrencode.encodeData('luaqrencode', 'l')

print('QR code version: ' .. version)
print('QR code width: ' .. width)
print('QR code data size: ' .. #data)

print('')

for n = 1, width do
	io.write('\t')
	for m = 1, width do
		local dot = string.sub(data, ((n - 1) * width) + m,
		    ((n - 1) * width) + m)
		io.write(dot)
	end
	io.write('\n')
end

print('')

-- Output the QR code as HTML file

local f = io.open('test-output.html', 'w')

f:write([[<html>
<head<>
<style>
table {
	border: 0px;
	border-spacing: 0;
	border-collapse:collapse;
}

td {
	border: 0px;
}

.box {
	width: 8px;
	height: 8px;
}

.black {
	background: #000;
}

.white {
	background: #fff;
}
</style>
</head>
<body>
<br><br><br><br><br><br>

<div align="center">
<table cellspacing="0" cellpadding="0">
]])

for n = 1, width do
	f:write('<tr>\n')
	for m = 1, width do
		local dot = string.sub(data, ((n - 1) * width) + m,
		    ((n - 1) * width) + m)
		if dot == '#' then
			f:write('\t<td><div class="box black"></div></td>\n')
		else
			f:write('\t<td><div class="box white"></div></td>\n')
		end
	end
	f:write('</tr>\n')
end

f:write([[
</table>
</div>
</body>
</html>
]])

f:close()
