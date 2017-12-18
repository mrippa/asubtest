[schematic2]
uniq 1
[tools]
[detail]
[cell use]
use bc200tr -144 -24 -100 0 frame
xform 0 1536 1280
use estringins 280 1480 100 0 estringins#5
xform 0 384 1552
p 368 1472 100 1024 -1 name:$(top)logrecord
use ebis 280 1800 100 0 ebis#4
xform 0 384 1872
p 272 1694 100 0 1 ONAM:SIMULATED
p 368 1792 100 1024 -1 name:$(top)TIME:intSimulate
p 272 1726 100 0 1 ZNAM:REAL
use esirs 280 2024 100 0 esirs#3
xform 0 464 2176
p 368 2016 100 1024 -1 name:$(top)TIME:health
p 384 2142 100 0 0 SNAM:
use ebis 1080 1848 100 0 ebis#7
xform 0 1184 1920
p 1072 1742 100 0 1 ONAM:ON
p 1168 1840 100 1024 1 name:$(top)TIME:debug
p 1072 1774 100 0 1 ZNAM:OFF
use eaSub 1616 1304 100 0 eaSub#8
xform 0 1760 1704
p 1728 1272 100 1024 1 name:$(top)aSub1
[comments]
