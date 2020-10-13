using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;


namespace XAPI
{
    [ComVisible(false)]
    public class PositionFieldEx
    {
        public string Symbol { get; private set; }
        public string Instrument { get; private set; }
        public string Exchange { get; private set; }

        public PositionField Long { get; private set; }
        public PositionField Short { get; private set; }

        public PositionFieldEx()
        {
            Long = new PositionField();
            Short = new PositionField();
        }

        public void AddPosition(PositionField position)
        {
            Symbol = position.Symbol;
            Instrument = position.InstrumentID;
            Exchange = position.ExchangeID;

            if (position.Side == PositionSide.Long)
            {
                Long = position;
            }
            else
            {
                Short = position;
            }
        }

        public double Qty
        {
            get { return Long.Position - Short.Position; }
        }
        public double LongQty
        {
            get { return Long.Position; }
        }
        public double ShortQty
        {
            get { return Short.Position; }
        }
    }
}
