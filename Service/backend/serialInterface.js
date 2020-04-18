const SerialPort = require('serialport');
const Readline = require('@serialport/parser-readline')
const CircularBuffer = require("circular-buffer");


const unsolicBuffer = CircularBuffer(1000);
const logBuffer = CircularBuffer(1000);
const port = new SerialPort('/dev/serial0', {
    baudRate: 460800
});
const parser = port.pipe(new Readline({ delimiter: '\n\r' }))
parser.on('data', function (data) {
    switch (data.toString()[0]) {
        case '%':
        {
            switch (data.toString()[1]) {
                case 'D':
                {
                    parseDebugMessage(data).then(msg =>
                    {
                        logBuffer.enq(msg)
                    })
                }
                break;
                default:
                {
                    unsolicBuffer.enq({time: new Date(), msg: data.toString().trim()})
                }
                break;
            }
        }
            break;
        case '#':
        {
            console.log('@', new Date(), ' REPLY:', data.toString().trim())
        }
            break;

    }
})

function parseDebugMessage(message)
{
    return new Promise(((resolve, reject) =>
    {
        if(message.length < 5)
        {
            reject(new Error("Message too short to be a valid debug message"));
        }
        else if (message[0] !== '%')
        {
            reject(new Error("Is not of unsolicited type"));
        }
        else if (message[1] !== 'D')
        {
            reject(new Error("Is not of debug type"));
        }
        else
        {
            var level = undefined;
            switch (message[2])
            {
                case 'D':
                {
                    level = 'debug';
                }
                break;
                case 'I':
                {
                    level = 'info';
                }
                break;
                case 'W':
                {
                    level = 'warning';
                }
                break;
                case 'E':
                {
                    level = 'error';
                }
                break;
                case 'C':
                {
                    level = 'critical';
                }
                break;
                default:
                {
                    reject(new Error("Unrecognised debug level"));
                }
                break;
            }
            if(level)
            {
                resolve({time: new Date(), level: level, msg: message.substring(4).trim()})
            }
        }
    }));
}

function getDebugLog(max)
{
    if(max)
    {
        return logBuffer.toarray().slice(0, max);
    }
    else
    {
        return logBuffer.toarray();
    }
}

function getCardLog(max)
{
    if(max)
    {
        return unsolicBuffer.toarray().slice(0, max);
    }
    else
    {
        return unsolicBuffer.toarray();
    }
}


module.exports =  {getDebugLog: getDebugLog, getCardLog: getCardLog}
port.write('\n\r');
port.write('#V?\n\r');