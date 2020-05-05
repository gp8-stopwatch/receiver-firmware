int main (void)
{
        HAL_Init ();
        SystemClock_Config ();

        Debug *d = Debug::singleton ();
        d->init (115200);
        d->print ("nRF24L01+ test here\n");

        //        Gpio ce1 (GPIOA, GPIO_PIN_2);
        //        ce1.set (false);

        Gpio irq1 (GPIOA, GPIO_PIN_0, GPIO_MODE_IT_RISING);

        HAL_NVIC_SetPriority (EXTI0_1_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ (EXTI0_1_IRQn);

        irq1.setOnToggle ([d] { d->print ("#"); });

        //        Spi spi1 (SPI1, GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_AF0_SPI1);

        //        uint8_t buff[5];
        //        int i = 0;
        //        spi1.setCallback ([&i, &buff](uint8_t c) { buff[i++] = c; });

        //        Nrf24L0P nrfTx (&spi1, &ce1, &irq1);

        //        Nrf24L0P nrfRx (&spi1, &ce1, &irq1);

        while (1) {
                //                screen->refresh ();
                //                buzzer->run ();
                //                button->run ();
        }
}
