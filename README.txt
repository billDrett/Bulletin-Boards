Dieukrinhseis:
 H epikoinwnia metaksy twn process ginetai apo 4 named pipe. 2 gia thn epikoinwnia tou server me ton client kai 2 gia thn epikoinwnia tou server me to boardPost ek ton opoio to ena einai gia read to allo gia write. Ta mnm stelnontai mesa se tags.
CLIENT SERVER MINIMATA
1) Otan thelei na steilei mnm createchannel to mnm exei authn thn morfh <Chanel><start>ID NAME<end><chanel>, etsi o server kserei apo to <Chanel> oti prepei na dimiourghsei neo chanel kai ta dedomena tou vriskontai anamesa sta tags <start> kai <end>
2) Otan thelei na steilei mnm getmessages to mnm exei thn morgh <Message><start> ID <end><Message> opou to id einai to chanel ID, o server kserei etsi oti prepei na steilei ta mnm sto pipe tou chanel ID. H apanthsh tou server einai morfhs <Message><start> .... <end><Message><File><:..><start> data<end><:...><start>data<end><File>.
Etsi o client kserei oti otan sinantisei to <Message> tote auto tha einai ta minimata pou teleiwnoun otan sinantisei to <end><Message>. Kai otan dei to <File> apo ekei kai pera einai arxeia, prin ta dedomena tou arxeiou dinetai to <:..> pou mesa tou periexei to onoma tou arxeiou. Oso vlepei oti yparxoun <:> tha sinexisei na apothikeuei ta arxeia mexri na dei to <File> opou tote stamata.
3)otan thelei na kanei shutdown stelnei to mnm <Shutdown> kai o server kleinei ta pipes tou kai termatizetai h diergasia.

BOARDPOST SERVER MINIMATA
1) Otan to boardPost stelnei mnm list sto server to mnm exei authn thn morfh <Chanels> kai o server apanta me <Chanel><start> ... <end><Chane>. Anamesa sto <start> kai <end> vriskontai ta chanels
2) To boardPost stelnei messages sto server. To mnm exei thn morfh <Message><start>*ID*...<end><Message>, opou to *ID* einai to chanel id pou tha apothikeutei to message
3) To boardPost stelnei files sto server. To mnm exei thn morfh <File><start>ID fileName ...<end><File>, opou ID einai to chanel id pou tha apothikeutei to file kai fileName einai to onoma tou arxeiou. Ta prwta 30 bytes mesa to <start> exoun to Id kai to fileName. Ama einai ligotera apo 30 gemizoume to ypoloipo me spaces.

Prokeimenou na anagnwrisoume ta dedomena twn minimatwn mpainoume se ena loop. Prwta perimenw na synanthsw to command (px <Chanel>, <Message>). Ama dn to vrw koitaw mipos exei kopei to tag kai pairnw apo ton xarakthra "<" mexri to telos tou buffer kai to vazw thn arxh tou buffer etsi wste sto epomeno read to tag na einai olokliro. Ama dn vrw "<" apla diavazw ksana apo to pipe. Meta perimeno to <start>, meta to start kanw kapoia energia mexri na sinanthsw to <end>. Tha mporousa na allaksw parapanw apo ena state mesa sto idio loop gia auto dn xrhsimopoihw if then else if

Gia na mporw na diavazw kai apo ta dyo pipes tautoxrona xrhsimopoihw thn select. Plirofories gia thn select pira apo edw http://stackoverflow.com/questions/14544621/why-is-select-used-in-linux

gia thn metaglwthsh apla ekteloume thn entolh make kai ftiaxnei ta ektelisa board pou einai ousiastika o client kai epikoinwnei me ton server gia na lavei mnm kai to boardPost pou einai gia na steiloume mnm kai arxeia se kapoio channel.

px ektelesimo
./board ./myboard
./board /home/bill/Desktop/myboard

./boardPost ./myboard
./boardPost /home/bill/Desktop/myboard

ama to path ektelestei me "/" sto telos dn tha doulepsei px ./myboard/ dn leitourgei
