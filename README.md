# Sliding-Window-Protocol

 Implemented Sliding Window Protocol in order to successfully transfer ﬁles (fileX in this project) using a link that can lose, reorder and corrupt data.
 
 Run the "run_experiment.sh" script in order to run the tests.
 
 1) Legatura de date nu pierde si nu corupe date
	-----------------------------------------------
		-pentru aceasta situatie a fost implementata o fereastra, mod
	eficient de a trimite cadre; fereastra este mereu plina, iar atunci 
	cand se primeste un ack, se trimite un alt mesaj;
		-dimensiunea ferestrei se calculeaza dupa formula 
	w = (BDP * 1000) / (MSGSIZE * 8), unde BDP reprezinta produsul dintre
	speed si delay, date transmise ca argumente;
		-pentru aflarea numarului total de cadre ce trebuie transmise
	a fost utilizata functia lseek (pentru a afla dimensiunea fisierului
	care se imparte la dimensiunea unui mesaj);
		-poate aparea situatia ca dimensiunea ferestrei sa fie mai mare
	decat numarul total de cadre, caz in care fereastra va fi chiar numarul
	de mesaje ce trebuie transmise.
	
	!Pentru rezolvarea urmatoarelor cerinte a fost creata o noua structura
	care contine un camp de tip intreg care reprezinta numarul de secventa
	al mesajului. Astfel noul payload va fi cu sizeof(int) mai mic.
	!!Mesajul cu numar de secventa 0 este cel care contine numele 
	fisierului de input (in payload) si numarul total de cadre (in len).

	2) Legatura de date poate pierde cadre
	--------------------------------------
		-pentru aceasta cerinta apare nevoia de a stoca mesajele pe 
	masura ce sunt citite intr-o structura, un vector de mesaje;
		-de asemenea, se creeaza un vector care verifica daca un mesaj
	a fost deja trimis, astfel daca trimis[i] are valoarea 1, mesaje[i] a
	fost trimis, altfel, acest mesaj urmeaza sa fie trimis; analog, in 
	recv.c apare un vector care verifica daca un mesaj a fost primit;
		-se parcurge vectorul care verifica trimiterea mesajelor si la
	prima valoarea de 0 pe care o intalneste incearca trimiterea mesajului
	corespunzator; este primit si se ia la cunostinta acest lucru prin
	marcarea cu 1 in vectorul de primite a mesajului cu numarul de secventa
	corespunzator.

	4) Legatura de date reordoneaza cadrele
	---------------------------------------
		-in cadrul acestei cerinte apare necesitatea de a stoca 
	mesajele pe masura ce sunt primite, pe pozitia data de numarul de 
	secventa al mesajului primit;
		-daca la momentul primirii fiecare mesaj a fost plasat pe 
	pozitia pe care a fost si la momentul trimiterii, o simpla parcurgere
	da ordinea corecta a mesajelor;
		-astfel se parcurg mesajele si se afiseaza pe rand in fisierul
	de output;
		-dupa ce s-au terminat mesajele, se trimite catre send un mesaj
	final care sa anunte ca s-a terminat scrierea, mesaj codificat cu numar
	de secventa infinit.	
