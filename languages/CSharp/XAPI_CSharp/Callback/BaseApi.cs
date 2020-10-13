//using NLog;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace XAPI.Callback
{
    [ComVisible(false)]
    public class BaseApi : IDisposable
    {
        // 没有必要专门引入一个库，但又很少用
        //public Logger Log;
        public object Log { get; set; }

        protected Proxy proxy;
        protected IntPtr Handle = IntPtr.Zero;
        public string LibPath;

        [CLSCompliant(false)]
        protected XCall _XRespone;

        public RspUserLoginField UserLogin { get; set; }
        public ServerInfoField Server { get; set; }
        public UserInfoField User { get; set; }
        public List<UserInfoField> UserList = new List<UserInfoField>();

        public DelegateOnConnectionStatus OnConnectionStatus
        {
            get { return OnConnectionStatus_; }
            set { OnConnectionStatus_ = value; }
        }
        public DelegateOnRtnError OnRtnError
        {
            get { return OnRtnError_; }
            set { OnRtnError_ = value; }
        }
        public DelegateOnLog OnLog
        {
            get { return OnLog_; }
            set { OnLog_ = value; }
        }
        private DelegateOnConnectionStatus OnConnectionStatus_;
        private DelegateOnRtnError OnRtnError_;
        private DelegateOnLog OnLog_;




        protected object locker = new object();

        public bool IsConnected { get; protected set; }
        private int _reconnectInterval;
        public int ReconnectInterval
        {
            get { return _reconnectInterval; }
            set
            {

                if (_reconnectInterval == value)
                    return;

                _reconnectInterval = value;
                _Timer.Elapsed -= _Timer_Elapsed;
                if (_reconnectInterval >= 10)
                {
                    _Timer.Elapsed += _Timer_Elapsed;
                    _Timer.Interval = _reconnectInterval * 1000;
                }
                _Timer.Enabled = _reconnectInterval >= 10;
            }
        }

        private System.Timers.Timer _Timer = new System.Timers.Timer();

        public BaseApi()
        {
            // 这里是因为回调函数可能被GC回收
            _XRespone = _OnRespone;

            ReconnectInterval = 0;
        }

        public BaseApi(string path)
            : this()
        {
            LibPath = path;
            Server = new ServerInfoField();
            User = new UserInfoField();
            UserLogin = new RspUserLoginField();
        }

        void _Timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            _Timer.Enabled = false;

            lock (locker)
            {
                // 定时检查是否需要销毁对象重连
                if (!IsConnected)
                {
                    Disconnect();
                    Connect();
                }
            }

            _Timer.Enabled = true;
        }


        ~BaseApi()
        {
            Dispose(false);
        }

        private bool disposed;

        // 一定要先调用API的，再调用队列的，否则会出错
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    // Free other state (managed objects).
                }
                // Free your own state (unmanaged objects).
                // Set large fields to null.
                Disconnect();
                disposed = true;
            }
            //base.Dispose(disposing);
        }



        public virtual void Connect()
        {
            lock (locker)
            {
                // XSpeed多次连接出问题，发现会生成多次
                if (proxy == null)
                {
                    proxy = new Proxy(LibPath);
                }

                Handle = proxy.XRequest((byte)RequestType.Create, IntPtr.Zero, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

                RegisterAndStart(Marshal.GetFunctionPointerForDelegate(_XRespone));

                IntPtr ServerIntPtr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(ServerInfoField)));
                Marshal.StructureToPtr(Server, ServerIntPtr, false);

                IntPtr UserListIntPtr = IntPtr.Zero;
                int count = UserList.Count;

                if (UserList.Count > 0)
                {
                    UserListIntPtr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(UserInfoField)) * count);

                    for (int i = 0; i < UserList.Count; ++i)
                    {
                        Marshal.StructureToPtr(UserList[i], new IntPtr(UserListIntPtr.ToInt64() + i * Marshal.SizeOf(typeof(UserInfoField))), false);
                    }
                }
                else
                {
                    count = 1;
                    UserListIntPtr = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(UserInfoField)));
                    Marshal.StructureToPtr(User, UserListIntPtr, false);
                }

                // 进行连接
                proxy.XRequest((byte)RequestType.Connect, Handle, IntPtr.Zero, 0, 0, ServerIntPtr, 0, UserListIntPtr, count, Marshal.StringToHGlobalAnsi(Path.GetTempPath()), 0);

                Marshal.FreeHGlobal(ServerIntPtr);
                Marshal.FreeHGlobal(UserListIntPtr);
            }


        }

        public virtual void Disconnect()
        {
            lock (locker)
            {
                _Timer.Enabled = false;

                IsConnected = false;

                if (proxy != null && Handle.ToInt64() != 0)
                {
                    proxy.XRequest((byte)RequestType.Disconnect, Handle, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                    proxy.XRequest((byte)RequestType.Release, Handle, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);

                    proxy.Dispose();
                }

                proxy = null;
                Handle = IntPtr.Zero;
            }

        }

        /// <summary>
        /// 这个功能主要是给非.NET语言在调用时使用
        /// </summary>
        public void GCCollect()
        {
            System.GC.Collect();
        }

        public void RegisterAndStart(IntPtr ptr1)
        {
            lock (this)
            {
                if (proxy != null)
                {
                    proxy.XRequest((byte)RequestType.Register, Handle, IntPtr.Zero, 0, 0, ptr1, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                }
            }
        }


        public ApiType GetApiTypes
        {
            get
            {
                if (proxy == null)
                {
                    proxy = new Proxy(LibPath);
                }
                return (ApiType)proxy.XRequest((byte)RequestType.GetApiTypes, IntPtr.Zero, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
            }
        }

        public string GetApiVersion
        {
            get
            {
                if (proxy == null)
                {
                    proxy = new Proxy(LibPath);
                }
                IntPtr ptr = proxy.XRequest((byte)RequestType.GetApiVersion, IntPtr.Zero, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                return Marshal.PtrToStringAnsi(ptr);
            }
        }

        public string GetApiName
        {
            get
            {
                if (proxy == null)
                {
                    proxy = new Proxy(LibPath);
                }
                IntPtr ptr = proxy.XRequest((byte)RequestType.GetApiName, IntPtr.Zero, IntPtr.Zero, 0, 0, IntPtr.Zero, 0, IntPtr.Zero, 0, IntPtr.Zero, 0);
                return Marshal.PtrToStringAnsi(ptr);
            }
        }

        private IntPtr _OnRespone(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            //try
            {
                // 队列过来的消息，如何处理？
                switch ((ResponseType)type)
                {
                    case ResponseType.OnConnectionStatus:
                        _OnConnectionStatus(double1, ptr1, size1);
                        break;
                    case ResponseType.OnRtnError:
                        _OnRtnError(ptr1);
                        break;
                    case ResponseType.OnLog:
                        _OnLog(ptr1);
                        break;
                    default:
                        return OnResponse(type, pApi1, pApi2, double1, double2, ptr1, size1, ptr2, size2, ptr3, size3);
                }
            }
            //catch(Exception ex)
            //{
            //    // 有些封装老是出错，没办法，只好报一个例外出来
            //    Console.WriteLine(ex.Message);
            //    Console.WriteLine(ex.StackTrace);
            //}


            return IntPtr.Zero;
        }

        protected virtual IntPtr OnResponse(byte type, IntPtr pApi1, IntPtr pApi2, double double1, double double2, IntPtr ptr1, int size1, IntPtr ptr2, int size2, IntPtr ptr3, int size3)
        {
            return IntPtr.Zero;
        }

        private void _OnConnectionStatus(double double1, IntPtr ptr1, int size1)
        {
            ConnectionStatus status = (ConnectionStatus)double1;
            // 连接状态更新
            // 这种写法是为了解决多账号登录时，先进行Done然后再Logining或Doing的问题
            if (ConnectionStatus.Disconnected == status
                || ConnectionStatus.Uninitialized == status)
            {
                IsConnected = false;
            }
            else if (ConnectionStatus.Done == status)
            {
                IsConnected = true;
            }

            RspUserLoginField obj = default(RspUserLoginField);
            if (size1 > 0)
            {
                obj = PInvokeUtility.GetObjectFromIntPtr<RspUserLoginField>(ptr1);
                UserLogin = obj;
            }

            if (OnConnectionStatus_ != null)
                OnConnectionStatus_(this, status, ref obj, size1);
        }

        private void _OnRtnError(IntPtr ptr1)
        {
            if (OnRtnError_ == null)
                return;

            ErrorField obj = PInvokeUtility.GetObjectFromIntPtr<ErrorField>(ptr1);

            OnRtnError_(this, ref obj);
        }

        private void _OnLog(IntPtr ptr1)
        {
            if (OnLog_ == null)
                return;

            LogField obj = PInvokeUtility.GetObjectFromIntPtr<LogField>(ptr1);

            OnLog_(this, ref obj);
        }
    }
}
