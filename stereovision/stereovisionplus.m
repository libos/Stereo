function varargout = stereovisionplus(varargin)
% Master Thesis: Real-Time Stereo Vision     Wim Abbeloos    May 2010
% Karel de Grote-Hogeschool University College, Belgium
%
% This function provides an easy to use Graphical User Interface to stereomatch.m
% A simplified graphical user interface is available in stereovision.m
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
                   'gui_OpeningFcn', @stereovisionplus_OpeningFcn, ...
                   'gui_OutputFcn',  @stereovisionplus_OutputFcn, ...
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


% --- Executes just before stereovisionplus is made visible.
function stereovisionplus_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to stereovisionplus (see VARARGIN)

% Choose default command line output for stereovisionplus
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes stereovisionplus wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = stereovisionplus_OutputFcn(hObject, eventdata, handles) 
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

if get(handles.checkbox4,'UserData') == 1
    profile on
end

handles.subpixel = get(handles.checkbox6,'UserData');

tic;
[handles.dispmap handles.costmap,handles.pcost,handles.wcost] = stereomatch( leftg, rightg, handles.WS, handles.D, handles.subpixel);
handles.time = 1000 * toc;

if get(handles.checkbox4,'UserData') == 1
    profile viewer
end

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

handles.cost = get(handles.checkbox3,'UserData');
if handles.cost(1) == 1
    figure(2), set(figure(2),'Name', 'Matching cost'), set(figure(2),'NumberTitle', 'off');
    imshow(handles.costmap, [0 10000]); colorbar;
end

handles.write = get(handles.checkbox2,'UserData');
if handles.write(1) == 1
    handles.dispmap2 = uint8(handles.dispmap .* get(handles.edit7,'UserData'));
    imwrite( handles.dispmap2, 'dispmap.png' );
end

if get(handles.checkbox5,'UserData') == 1
    handles.X = get(handles.edit8,'UserData');
    handles.Y = get(handles.edit9,'UserData');
    figure(3), set(figure(3),'Name', 'Pixel cost plot'), set(figure(3),'NumberTitle', 'off');
    subplot(211); bar(0:handles.D,shiftdim(handles.pcost(handles.X,handles.Y,:))); xlim([-1 handles.D+1]); xlabel('Disparity [pixels]'); ylabel('Absolute intensity differences'); title('Single pixel match cost');
    h = findobj(gca,'Type','patch');
    set(h,'FaceColor',[0.8 0.8 0.8],'EdgeColor','k');
    subplot(212); bar(0:handles.D,shiftdim(handles.wcost(handles.X,handles.Y,:))); xlim([-1 handles.D+1]); xlabel('Disparity [pixels]'); ylabel('Sum of absolute differences'); title('Window match cost');
    h = findobj(gca,'Type','patch');
    set(h,'FaceColor',[0.8 0.8 0.8],'EdgeColor','k');
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
    myval = 4; set(handles.edit7,'UserData', myval); set(handles.edit7,'String', int2str(myval));
case 'Tsukuba'
    mystring = 'left2.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right2.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 11; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 15; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
    myval = 16; set(handles.edit7,'UserData', myval); set(handles.edit7,'String', int2str(myval));
case 'Cones'
    mystring = 'left3.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right3.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 21; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 59; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
    myval = 4; set(handles.edit7,'UserData', myval); set(handles.edit7,'String', int2str(myval));
case 'Venus'
    mystring = 'left4.png'; set(handles.edit3,'UserData',mystring); set(handles.edit3,'String',mystring);
    mystring = 'right4.png'; set(handles.edit4,'UserData',mystring); set(handles.edit4,'String',mystring);
    myval = 25; set(handles.edit1,'UserData', myval); set(handles.edit1,'String', int2str(myval));
    myval = 19; set(handles.edit2,'UserData', myval); set(handles.edit2,'String', int2str(myval));
    myval = 8; set(handles.edit7,'UserData', myval); set(handles.edit7,'String', int2str(myval));
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

% --- Executes on button press in checkbox2.
function checkbox2_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox2

myval = get(handles.checkbox2,'Value');
set(handles.checkbox2,'UserData',myval);
guidata(hObject,handles);

function edit7_Callback(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit7 as text
%        str2double(get(hObject,'String')) returns contents of edit7 as a double

myval= str2double(get(handles.edit7,'String'));
set(handles.edit7,'UserData',myval);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit7_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in checkbox3.
function checkbox3_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox3

myval = get(handles.checkbox3,'Value');
set(handles.checkbox3,'UserData',myval);
guidata(hObject,handles);

% --- Executes on button press in checkbox4.
function checkbox4_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox4

myval = get(handles.checkbox4,'Value');
set(handles.checkbox4,'UserData',myval);
guidata(hObject,handles);

% --- Executes on button press in checkbox5.
function checkbox5_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox5

myval = get(handles.checkbox5,'Value');
set(handles.checkbox5,'UserData',myval);
guidata(hObject,handles);

function edit8_Callback(hObject, eventdata, handles)
% hObject    handle to edit8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit8 as text
%        str2double(get(hObject,'String')) returns contents of edit8 as a double

myval = str2double(get(handles.edit8,'String'));
set(handles.edit8,'UserData',myval);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit8_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function edit9_Callback(hObject, eventdata, handles)
% hObject    handle to edit9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit9 as text
%        str2double(get(hObject,'String')) returns contents of edit9 as a double

myval = str2double(get(handles.edit9,'String'));
set(handles.edit9,'UserData',myval);
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit9_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

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

% --- Executes on button press in checkbox6.
function checkbox6_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox6

myval = get(handles.checkbox6,'Value');
set(handles.checkbox6,'UserData',myval);
guidata(hObject,handles);