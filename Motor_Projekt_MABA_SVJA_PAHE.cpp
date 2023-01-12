// SES6-02b-UDP-C.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <windows.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")


int main(void)
{
	//Verbindungsdaten-Variabeln mit Default werten erzeugen
	int PORT = 1000; //Server Port
	string SERVERADDRESS = "192.168.1.102"; //server adresse
	const int MESSAGELENGTH = 128; //max Länger der Nachricht

	//Ticket rechts aus string erstellen
	string ticketSR = "'<control pos=\"0\" speed=\"3000\" torque=\"8000\" mode=\"130\" acc=\"60000\" decc=\"60000\" />'";
	char ticketCR[MESSAGELENGTH];
	//Konvertierung von String Ticket zu Char Ticket
	strcpy(ticketCR, ticketSR.c_str());

	//Ticket rechts aus string erstellen
	string ticketSL = "'<control pos=\"0\" speed=\"-3000\" torque=\"8000\" mode=\"130\" acc=\"60000\" decc=\"60000\" />'";
	char ticketCL[MESSAGELENGTH];
	//Konvertierung von String Ticket zu Char Ticket
	strcpy(ticketCL, ticketSL.c_str());

	//Zustand Hilfsvariabeln
	bool toBeClosed = false;

	//Verbindugnsdaten von User abfragen
	cout << "Bitte Motor IP eingeben\n";
	cin >> SERVERADDRESS;
	cout <<"Bitte Motor Port eingeben\n";
	cin >>PORT;

	SOCKET tcpSocket;	//Socket Objekt erstellen, benötigt für socket()
	struct sockaddr_in serverAdress, clientAddress; //Structs in denen wir die Daten zum Server und Client Speichern
	int slen, recv_len; //Variablen gebraucht in sendto() und recvfrom()
	//char message[MESSAGELENGTH];	//in message speichern wir die erhaltenen/versendeten Daten

	WSADATA wsaData;	//WSADATA Objekt, benötigt für WSAStartup()
	slen = sizeof(clientAddress); //Länge der Client Adresse, gebruacht in sendto()

	//Winsock initialisieren
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup fehlgeschlagen: " << WSAGetLastError() << endl;
		system("pause");
		return 1;
	}

	//TCP Socket erstellen
	if ((tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cout << "Socket erstellen fehlgeschlagen: " << WSAGetLastError() << endl;
		system("pause");
		return 1;
	}
	
		//Speicher reservieren
		memset((char*)&serverAdress, 0, sizeof(serverAdress)); //Reserviert Platz im Memory für die Daten
		serverAdress.sin_family = AF_INET;
		serverAdress.sin_port = htons(PORT);
		serverAdress.sin_addr.S_un.S_addr = inet_addr(SERVERADDRESS.c_str());

		
		//Verbinden
		if (connect(tcpSocket, (struct sockaddr*)&serverAdress, slen) == INVALID_SOCKET)
		{
			cout << "Verbindung herstellen fehlgeschlagen: " << WSAGetLastError() << endl;
			system("pause");
			return 1;
		}
		cout << "Verbunden mit dem Server: " << inet_ntoa(serverAdress.sin_addr) << endl;
		cout << "Bitte gewuenschte Taste druecken:\nW -> Drehung Rechts\nS -> Drehung Links\nA -> Programm beenden\n";

		//Schleife bis durch A Drücken Socket schliessen getriggert wird
		while (toBeClosed == false) {

			//Daten senden

			//Ticket zum Rechtsdrehen senden
			if (GetAsyncKeyState('W') & 0x8000) {

				if (send(tcpSocket, ticketCR, strlen(ticketCR), 0) == SOCKET_ERROR)
				{
					cout << "Daten senden fehlgeschlagen: " << WSAGetLastError() << endl;
					system("pause");
					return 1;
				}
				cout << "Ticket rechts gesendet.\n";
				//Verhindern dass pro Keystroke mehrere Tickets gesendet werden
				while (GetAsyncKeyState('W') & 0x8000) {}
			}
			//Ticket zum Linksdrehen senden
			else if (GetAsyncKeyState('S') & 0x8000) {
				if (send(tcpSocket, ticketCL, strlen(ticketCL), 0) == SOCKET_ERROR)
				{
					cout << "Daten senden fehlgeschlagen: " << WSAGetLastError() << endl;
					system("pause");
					return 1;
				}
				cout << "Ticket links gesendet.\n";
				//Verhindern dass pro Keystroke mehrere Tickets gesendet werden
				while (GetAsyncKeyState('S') & 0x8000) {}
			}
			//aus Schleife springen
			else if (GetAsyncKeyState('A') & 0x8000) {
				toBeClosed = true;
			}
		}

	// Socket schliessen
	closesocket(tcpSocket);
	WSACleanup();

	system("pause");
	return 0;
}
