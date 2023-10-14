# Kontrola kontrolního hlášení

Úkolem je realizovat třídy, které implementují správu daně z přidané hodnoty.

Předpokládáme, že firmy si předávají faktury. Firma, která vydá fakturu, musí z fakturované částky odvést státu DPH. Analogicky, firma, která přijme (a zaplatí fakturu) může po státu nárokovat vratku DPH (za určitých okolností). Právě vracení DPH je slabým místem, které lze využít ke krácení daně a daňovým podvodům. Proto je úkolem implementovat jádro systému, který bude takové krácení daně ztěžovat.

Vydané a přijaté faktury eviduje třída CVATRegister. Firmy musí registrovat vydávané a přijímané faktury. Registr páruje dvojice sobě odpovídajících faktur a je schopen najít faktury nespárované. Fakturu lze do registru jednak přidat a dále i zrušit (např. při chybně zadaných údajích). Rozhraní třídy je následující:


- implicitní konstruktor:
    inicializuje prázdnou instanci registru,
- registerCompany ( name ):
    metoda zavede zadanou firmu do registru. Předané jméno je oficiální název firmy, toto jméno bude používané v exportech z registru. Návratovou hodnotou je indikátor úspěchu (true)/neúspěchu (false). Za neúspěch považujte, pokud v registru již existuje firma stejného jména. Při porovnávání jména firmy je registr docela tolerantní:

        - při porovnávání nerozlišuje malá a velká písmena,
        - při porovnávání neuvažuje nadbytečné mezery.

Tato pravidla jsou používána při zakládání nové firmy i vkládání / mazání faktur. Například názvy "My Company", "mY CoMpAnY", " My Company " a " mY CoMPAnY " jsou považované za jednu firmu, ale názvy "My Company" a "MyCompany" označují dvě rozdílné firmy.
- addIssued ( invoice )
    metoda přidá fakturu do registru. Tuto metodu volá firma, která fakturu vydala. Návratovou hodnotou je příznak úspěch (true)/neúspěch (false). Za chybu je považováno, pokud prodávající / kupující ve faktuře nejsou registrované, prodávající a kupující je ta samá firma nebo pokud stejná faktura již byla pomocí addIssued zadaná (dvě faktury se musí lišit alespoň v jednom z: prodávající/kupující/datum/částka/DPH).
- addAccepted ( invoice )
    metoda přidá fakturu do registru, tuto metodu volá firma, která fakturu přijala (kupující). Jinak se metoda chová stejně jako addIssued.
- delIssued ( invoice )
    metoda odebere fakturu z registru. Tuto metodu volá firma, která fakturu vydala a dříve zaregistrovala. Návratovou hodnotou je příznak úspěch (true)/neúspěch (false). Za chybu je považováno, pokud identická faktura nebyla dříve registrovaná metodou addIssued.
- delAccepted ( invoice )
    metoda odebere fakturu z registru. Tuto metodu volá firma, která fakturu přijala a dříve zaregistrovala. Návratovou hodnotou je příznak úspěch (true)/neúspěch (false). Za chybu je považováno, pokud identická faktura nebyla dříve registrovaná metodou addAccepted.
- unmatched ( company, sortOpt )
    metoda nalezne všechny faktury, které se týkají zadané firmy company a nebyly spárované (tedy byly registrované pouze pomocí addIssued nebo pouze pomocí addAccepted). Metoda vrátí seznam těchto faktur, faktury budou seřazené podle kritérií udaných sortOpt. Faktury vrácené touto metodou budou mít na místě názvu firmy "oficiální" název, tedy ten název, který byl zadán při registraci firmy metodou registerCompany. Tento oficiální název bude rovněž použit při řazení.

Třída CInvoice reprezentuje jednu fakturu. Rozhraní musí splňovat:
- konstruktor ( date, seller, buyer, amount, vat )
    inicializace fakturu datem, jménem prodávající a kupující firmy, fakturovanou částkou a DPH.
- date, seller, buyer, amount, vat
    přístupové metody ke čtení jednotlivých složek faktury.
- další
    do rozhraní faktury si můžete doplnit další veřejné/neveřejné metody a členské proměnné, které pro implementaci budete potřebovat.

Třída CSortOpt určuje kritéria pro řazení. Pro řazení lze použít všechny složky faktury. Pokud například vytvoříme instanci:

    CSortOpt () . addKey ( CSortOpt::BY_AMOUNT, true ) . addKey ( CSortOpt::BY_SELLER, false )
pak se řadí podle fakturované částky vzestupně (první řadicí kritérium) a pro stejné hodnoty fakturované částky se použije řazení podle jména prodávajícího sestupně (druhé řadicí kritérium). Pokud by ani takto nebylo pořadí jednoznačně určené, použije se jako řadicí kritérium pořadí zavedení faktury do registru. Rozhraní třídy CSortOpt je:
-  implicitní konstruktor
    inicializuje prázdnou instanci třídy
- addKey ( sortBy, ascending )
    přidá další řadicí kritérium sortBy, směr řazení je daný příznakem ascending (true = vzestupně, false = sestupně). Řadit lze podle:
- BY_DATE - podle data faktury,
- BY_SELLER - podle oficiálního jména prodávající firmy, řadí se bez ohledu na malá/velká písmena,
- BY_BUYER - podle oficiálního jména kupující firmy, řadí se bez ohledu na malá/velká písmena,
- BY_AMOUNT - podle fakturované částky,
- BY_VAT - podle DPH