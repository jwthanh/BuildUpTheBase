<GameFile>
  <PropertyGroup Name="achievement_scene" Type="Scene" ID="eeb6bcf9-3a08-476d-9ab5-c988a81f43da" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Scene" Tag="241" ctype="GameNodeObjectData">
        <Size X="960.0000" Y="640.0000" />
        <Children>
          <AbstractNodeData Name="panel" ActionTag="368919939" Tag="242" IconVisible="False" TopMargin="0.0003" BottomMargin="-0.0003" TouchEnable="True" ClipAble="False" BackColorAlpha="102" ComboBoxIndex="1" ColorAngle="90.0000" Scale9Width="1" Scale9Height="1" ctype="PanelObjectData">
            <Size X="960.0000" Y="640.0000" />
            <Children>
              <AbstractNodeData Name="back_btn" ActionTag="1553396072" Tag="243" IconVisible="False" LeftMargin="35.0000" RightMargin="795.0000" TopMargin="500.0000" BottomMargin="40.0000" TouchEnable="True" FontSize="24" ButtonText="Back" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="16" Scale9Height="14" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
                <Size X="130.0000" Y="100.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="100.0000" Y="90.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.1042" Y="0.1406" />
                <PreSize X="0.1354" Y="0.1563" />
                <FontResource Type="Normal" Path="pixelmix.ttf" Plist="" />
                <TextColor A="255" R="65" G="65" B="70" />
                <DisabledFileData Type="Default" Path="Default/Button_Disable.png" Plist="" />
                <PressedFileData Type="Default" Path="Default/Button_Press.png" Plist="" />
                <NormalFileData Type="Default" Path="Default/Button_Normal.png" Plist="" />
                <OutlineColor A="255" R="255" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="title_lbl" ActionTag="-884361543" Tag="244" IconVisible="False" LeftMargin="339.9999" RightMargin="340.0001" TopMargin="30.2877" BottomMargin="577.7123" LabelText="Achievements" ctype="TextBMFontObjectData">
                <Size X="280.0000" Y="32.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="479.9999" Y="593.7123" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.5000" Y="0.9277" />
                <PreSize X="0.2917" Y="0.0500" />
                <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_32x4.fnt" Plist="" />
              </AbstractNodeData>
              <AbstractNodeData Name="desc_lbl" ActionTag="536310315" Tag="245" IconVisible="False" LeftMargin="479.9999" RightMargin="480.0001" TopMargin="99.2877" BottomMargin="540.7123" LabelText="" ctype="TextBMFontObjectData">
                <Size X="0.0000" Y="0.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="479.9999" Y="540.7123" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="0" B="0" />
                <PrePosition X="0.5000" Y="0.8449" />
                <PreSize X="0.0000" Y="0.0000" />
              </AbstractNodeData>
              <AbstractNodeData Name="achievement_listview" ActionTag="-152339019" Tag="246" IconVisible="False" LeftMargin="182.9997" RightMargin="-622.9998" TopMargin="137.0004" BottomMargin="-422.0004" TouchEnable="True" ClipAble="True" BackColorAlpha="102" ComboBoxIndex="1" ColorAngle="90.0000" Scale9Width="1" Scale9Height="1" IsBounceEnabled="True" ScrollDirectionType="0" DirectionType="Vertical" ctype="ListViewObjectData">
                <Size X="1400.0000" Y="925.0000" />
                <AnchorPoint ScaleY="1.0000" />
                <Position X="182.9997" Y="502.9996" />
                <Scale ScaleX="0.5000" ScaleY="0.5000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.1906" Y="0.7859" />
                <PreSize X="1.4583" Y="1.4453" />
                <SingleColor A="255" R="150" G="150" B="255" />
                <FirstColor A="255" R="150" G="150" B="255" />
                <EndColor A="255" R="255" G="255" B="255" />
                <ColorVector ScaleY="1.0000" />
              </AbstractNodeData>
              <AbstractNodeData Name="unlocked_lbl" ActionTag="-918996730" Tag="375" IconVisible="False" LeftMargin="13.0000" RightMargin="783.0000" TopMargin="252.2500" BottomMargin="339.7500" LabelText="Unlocked:&#xA;123,456QuD" ctype="TextBMFontObjectData">
                <Size X="164.0000" Y="48.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="95.0000" Y="363.7500" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.0990" Y="0.5684" />
                <PreSize X="0.1708" Y="0.0750" />
                <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
              </AbstractNodeData>
              <AbstractNodeData Name="remaining_lbl" ActionTag="-1171744398" Tag="376" IconVisible="False" LeftMargin="12.9999" RightMargin="783.0001" TopMargin="349.7500" BottomMargin="242.2500" LabelText="Remaining:&#xA;123,456QuD" ctype="TextBMFontObjectData">
                <Size X="164.0000" Y="48.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="94.9999" Y="266.2500" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.0990" Y="0.4160" />
                <PreSize X="0.1708" Y="0.0750" />
                <LabelBMFontFile_CNB Type="Normal" Path="pixelmix_24x2.fnt" Plist="" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleY="1.0000" />
            <Position Y="639.9997" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition Y="1.0000" />
            <PreSize X="1.0000" Y="1.0000" />
            <SingleColor A="255" R="150" G="200" B="255" />
            <FirstColor A="255" R="150" G="200" B="255" />
            <EndColor A="255" R="255" G="255" B="255" />
            <ColorVector ScaleY="1.0000" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>