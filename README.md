# SysPro2

<p><h3>Compile</h3></p>
make

<h3><p>Usage</h3></p>
./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir

<h3><p>Description</h3></p>
 TravelMonitor will generate numMonitors processes, Every process Monitor is executing the application VaccineMonitor (see project SysPro1-VaccinateMonitor). 
 For the commands /travelRequest, /searchVaccinationStatus, /addVaccinationRecords, travelMonitor will send some request via named pipes to Monitors.
 Monitor will extract the answer. The answer will be sent back to travelMonitor via named pipe, at the end travelMonitor will print out the answer.
 
 
<h3><p>Commands</h3></p>

<p>/travelRequest citizenID date countryFrom countryTo virusName <br/>
print No/Yes</p>

/travelStats virusName date1 date2 country  </br>
print all vaccinations for the citizen with citizenID


/travelStats virusName date1 date2 </br>



/addVaccinationRecords country </br>

/searchVaccinationStatus citizenID </br>



/exit </br>
Exit from application </br>
kill all children with SIGKILL </br>
close named pipes </br>
free memory

