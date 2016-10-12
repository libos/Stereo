function varargout = stereovision(varargin)
% Master Thesis: Real-Time Stereo Vision     Wim Abbeloos    May 2010
% Karel de Grote-Hogeschool University College, Belgium
%
% This function provides an easy to use Graphical User Interface to stereomatch.m
% A more complete graphical user interface is available in
% stereovisionplus.m
% 
% Please note stereomatch.m requires the Image Processing Toolbox!
%
% The standard images included are from
% [1] 	D. Scharstein and R. Szeliski. A taxonomy and evaluation of dense two-frame stereo correspondence algorithms.
% International Journal of Computer Vision, 47(1/2/3):7-42, April-June 2002.
% [2] 	D. Scharstein and R. Szeliski. High-accuracy stereo depth maps using structured light.
% In IEEE Computer Society Conference on Computer Vision and Pattern
% Recognition (CVPR 2003), volume 1, pages 195-202, Madison, WI, June 2003. 

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @stereovision_OpeningFcn, ...
                   'gui_OutputFcn',  @stereovision_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before stereovision is made visible.
function stereovision_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to stereovision (see VARARGIN)

% Choose default command line output for stereovision
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes stereovision wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = stereovision_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;

axis off;
% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

handles.left = get(handles.edit3,'UserData');
handles.right = get(handles.edit4,'UserData');
handles.WS = get(handles.edit1,'UserData');
handles.D = get(handles.edit2,'UserData');

handles.channel=get(handles.popupmenu2,'UserData');
left=imread(handles.left); right=imread(handles.right);
leftg=left(:,:,handles.channel); rightg=right(:,:,handles.channel);

tic;
[handles.dispmap handles.costmap,handles.pcost,handles.wcost] = stereomatch( leftg, rightg, handles.WS, handles.D, 0);
handles.time = 1000 * toc;

mystring = int2str( handles.time );
set(handles.text12, 'String', mystring);

imshow(handles.dispmap, [0 handles.D]);
if get(handles.radiobutton2, 'Value') == 1
    colormap('jet'); colorbar;
else
    colormap('gray'); colorbar;
end

handles.reconstruction = get(handles.checkbox1,'UserData');
if handles.reconstruction(1) == 1
    figure(1), set(figure(1),'Name', '3D reconstruction'), set(figure(1),'NumberTitle', 'off'); axis off;
    surface( single(handles.D) - handles.dispmap, left, 'FaceColor', 'texturemap', 'EdgeColor', 'none' );
end

guidata(hObject,handles);

function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double

myval = str2double(get(handles.edit1,'String'));
set(handles.edit1,'UserData',myval);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function right=edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double

myval = str2double(get(handles.edit2,'String'));
set(handles.edit2,'UserData',myval);
guidata(hObject,handles);


% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double

mystring = get(handles.edit3,'String');
set(handles.edit3,'UserData',mystring);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit4_Callback(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit4 as text
%        str2double(get(hObject,'String')) returns contents of edit4 as a double

mystring = get(handles.edit4,'String');
set(handles.edit4,'UserData',mystring);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in checkbox1.
function checkbox1_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox1

myval = get(handles.checkbox1,'Value');
set(handles.checkbox1,'UserData',myval);
guidata(hObject,handles);

% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1

str = get(hObject, 'String');
val = get(hObject,'Value');
% Set current data to the selected data set.
switch str{val};
case 'Teddy'
    mystring = 'left1.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right1.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 21; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 59; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
case 'Tsukuba'
    mystring = 'left2.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right2.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 11; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 15; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
case 'Cones'
    mystring = 'left3.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right3.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 21; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 59; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
case 'Venus'
    mystring = 'left4.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right4.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 25; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 19; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
end
guidata(hObject,handles)

% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on selection change in popupmenu2.
function popupmenu2_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu2 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu2

str = get(hObject, 'String');
val = get(hObject,'Value');
% Set current data to the selected data set.
switch str{val};
case 'Red (1)'
    myval = 1; set(handles.popupmenu2,'UserData', myval);
case 'Green (2)'
    myval = 2; set(handles.popupmenu2,'UserData', myval);
case 'Blue (3)'
    myval = 3; set(handles.popupmenu2,'UserData', myval);
end
guidata(hObject,handles)

% --- Executes during object creation, after setting all properties.
function popupmenu2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in radiobutton1.
function radiobutton1_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton1

colormap('gray');
set(handles.radiobutton2,'Value',0);

% --- Executes on button press in radiobutton2.
function radiobutton2_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton2

colormap('jet');
set(handles.radiobutton1,'Value',0);

% --- Executes on button press in pushbutton4.
function pushbutton4_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

handles.left = get(handles.edit3,'UserData');
handles.right = get(handles.edit4,'UserData');
left=imread(handles.left); right=imread(handles.right);
figure(4), set(figure(4),'Name', 'Current images'), set(figure(4), 'NumberTitle', 'off');
subplot(121); imshow(left); title('Reference image');subplot(122); imshow(right); title('Target image')