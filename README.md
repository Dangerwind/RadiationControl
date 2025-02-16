<div>
  <img src="https://github.com/devicons/devicon/blob/master/icons/c/c-original.svg" title="C" alt="C" width="40" height="40"/> &nbsp
</div>

## Пороговый сигнализатор ионизирующего излучения с автоматической настройкой

**Язык программирования:** <span style="color:#1F1F00;"> Си </span> (CodeVisionAVR) с элементами ассемблера.

**Описание:** Устройство анализирует естественный радиационный фон. Если уровень радиации превысит определенный порог, устройство начнет издавать тревожный звуковой и световой сигналы.
**Структура, логика и алгоритмы:** Программа непрерывно генерирует управляющие импульсы высоковольтного генератора и следит за величиной напряжения (400 вольт для питания газоразрядников, счетчиков Гейгера) реализуя ПИД. После включения, первые 20 секунд программа вычисляет математическое ожидание от естественного радиационного фона, далее переходит в режим контроля. В этом режиме непрерывно вычисляется скользящее среднее измеренного радиационного фона, и, если фон выходит за расчетные пороги "сигмы", включается генерация звукового сигнала и зажигается тревожный светодиод. 

**Комментарии:** Данное устройство входит в состав контрольно-досмотрой техники.

**Схемотехника:** микроконтроллер Atmega8L, счетчики Гейгера СБМ-19
  
 Мой первый коммерческий проект в разработке устройств на микроконтроллерах.

 ![Внешний вид устройства сигрализатора ионизирующего излучения](https://github.com/Dangerwind/RadiationControl/blob/main/img/RadiationControl.jpg)

