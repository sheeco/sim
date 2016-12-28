
>***Created by Eco Sun on 2015-11-03***


### Simulation Results

Log files in `/log`:

	final.log
	parameters.log

	delivery-ratio.log
	delivery-ratio-100.log
	delay.log
	energy-consumption.log
	activation.lgo
	buffer.log
	buffer-statistics.log
	encounter.log
	transmit.log
	hop.log

	hotspot.log
	at-hotspot.log

	hotspot-similarity.log
	hotspot-rank.log
	delivery-hotspot.log
	delivery-statistics.log
	merge.log
	merge-details.log
	ma.log
	buffer-ma.log
	ed.log


### Usage

- 默认配置文件 [/bin/default.config](/bin/default.config)；
- 使用命令行进行参数配置的规则见文件 [/bin/help.md](/bin/help.md)，或运行 `HDC -help` 查看；


### Dependency

	...

	
### Environment

- OS: Windows 10
- IDE: Visual Studio Enterprise 2015 Update 2 ( 14.0.25123.00 )
- C++ Standard: Microsoft Visual C++ 2015 ( 00322-90150-00888-AA891 )
- Platform Toolset: Visual Studio 2012 (v110)


### File List

    README.md
    
    HELP.md
    
    version.h & sim.rc

    Global.h & .cpp
    
    main.cpp
    
    ---
    
    Entity.h
    
    |-- BasicEntity.h & .cpp

		|-- GeneralData.h & .cpp

			|-- Frame.h & .cpp

			|-- Packet.h & .cpp

				|-- Ctrl.h & .cpp

				|-- Data.h & .cpp

		|-- GeneralNode.h & .cpp

			|-- Sink.h & .cpp
    
			|-- Node.h & .cpp
    
			|-- MANode.h & .cpp

    |-- GeoEntity.h & .cpp
    
		|-- Coordinate.h & .cpp
    
		|-- CTrace.h & .cpp
    
		|-- Position.h & .cpp
    
		|-- Hotspot.h & .cpp

		|-- Route.h & .cpp
    
    Process.h

    |-- Algorithm.h
    
		|-- Protocol.h & .cpp

			|-- RoutingProtocol.h & .cpp

				|-- Prophet.h & .cpp
	
				|-- HAR.h & .cpp

				|-- Epidemic.h & .cpp
	    
			|-- MacProtocol.h & .cpp
	
				|-- SMac.h & .cpp
	
				|-- HDC.h & .cpp

		|-- HotspotSelect.h & .cpp
    
		|-- PostSelect.h & .cpp
    
		|-- NodeRepair.h & .cpp

    |-- Helper.h & .cpp
    
		|-- FileHelper.h & .cpp
    
		|-- SortHelper.h & .cpp
    
		|-- ParseHelper.h & .cpp
    
		|-- RunHelper.h & .cpp
    
	Configuration.h & .cpp
        


### Interface

	...


