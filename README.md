# Projekt_Inzynierski_Odbiornik_Robot_Mobilny

W  powyższych załącznikach znajduje się podstawowa wersja programu. Pozwala ona transfer obrazu z kamer oraz posiada przykładowe wykorzystanie klasy "Custom Data IO" w którym komputery wymieniają się zwiększającymi się wartościami typu "int" i "float".

Aby móc korzystać z tego rozwiązania należy mieć zainstalowane następujące programy:
\begin{itemize}
\item kompilator języka c++
\item CMake \url{https://cmake.org/} 
\item SFML \url{https://www.sfml-dev.org/download.php/}
\item OpenCV \url{https://opencv.org/}
\end{itemize}

Aby skompilować stworzony projekt należy w pierwszej kolejności wykorzystać program CMake który to generuje plik które mogą być następnie obsłużone przez kompilator

Powyższe załączniki zawierają w sobie zmienioną wersję kodu. Pozwala ona na sterowanie robotem mobilnym który to został opisany w podrozdziałach \ref{sec:OpisPrzykładowejImplementacji} oraz \ref{sec:TestySystemu}.

Aby móc skompilować kod tego programu, na urządzeniu znajdującym się na pojeździe, musi być zainstalowana biblioteka WiringPi \url{http://wiringpi.com/}
